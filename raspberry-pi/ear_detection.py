import cv2
import dlib
import numpy as np
import threading
import time
import requests
from imutils import face_utils
from collections import deque


# Path to the 68 facial landmark predictor
PREDICTOR_PATH = "shape_predictor_68_face_landmarks.dat"


# Initialize dlib's face detector (HOG-based) and then the landmark predictor
face_detector = dlib.get_frontal_face_detector()
landmark_predictor = dlib.shape_predictor(PREDICTOR_PATH)


# Landmark indices
LEFT_EYE_INDICES  = [36, 37, 38, 39, 40, 41]
RIGHT_EYE_INDICES = [42, 43, 44, 45, 46, 47]
MOUTH_INDICES     = [60, 61, 62, 63, 64, 65, 66, 67]


# ESP32-CAM stream URL
ESPURL =  'http://192.168.1.4:81/stream' # ESP_IP
#ESPURL = 'http://10.2.16.231:81/stream'
#ESP32_POST_URL = 'http://10.8.16.141:5000/report'  # Example endpoint
ESP32_POST_URL = 'http://192.168.1.3:5000/report' #pi's ip
buzz = 'http://192.168.1.4/alert' # ESP_IP

class ESP32Stream:
    def __init__(self, url):
        self.cap = cv2.VideoCapture(url)
        self.ret, self.frame = self.cap.read()
        self.lock = threading.Lock()
        self.running = True
        threading.Thread(target=self.update, daemon=True).start()

    def update(self):
        while self.running:
            ret, frame = self.cap.read()
            if ret:
                with self.lock:
                    self.ret = ret
                    self.frame = frame


    def read(self):
        with self.lock:
            if not self.ret:
                return False, None
            return self.ret, self.frame.copy()


    def release(self):
        self.running = False
        self.cap.release()


def eye_aspect_ratio(eye):
    A = np.linalg.norm(eye[1] - eye[5])
    B = np.linalg.norm(eye[2] - eye[4])
    C = np.linalg.norm(eye[0] - eye[3])
    return (A + B) / (2.0 * C)


def mouth_aspect_ratio(mouth):
    A = np.linalg.norm(mouth[3] - mouth[7])
    B = np.linalg.norm(mouth[2] - mouth[6])
    C = np.linalg.norm(mouth[1] - mouth[5])
    D = np.linalg.norm(mouth[0] - mouth[4])
    return (A + B + C) / (3.0 * D)


def compute_fatigue_score(blinks, long_blinks, yawns):
    WEIGHTS = {
        "blinks": 1.0,
        "long_blinks": 4.2,
        "yawns": 7.5,
    }
    return (blinks * WEIGHTS["blinks"] +
            long_blinks * WEIGHTS["long_blinks"] +
            yawns * WEIGHTS["yawns"])


def send_fatigue_score_to_esp32(score):
    try:
        requests.post(ESP32_POST_URL, json={"score": score})
        print(f"Sent fatigue score {score:.1f} to ESP32")
    except Exception as e:
        print(f"Failed to send score: {e}")

def send_event_message(message):
    try:
        requests.post(ESP32_POST_URL, json={"message": message})
        print(f"Sent message to ESP32")
    except Exception as e:
        print(f"Failed to send message")


def main():
    stream = ESP32Stream(ESPURL)


    EAR_THRESHOLD = 0.22
    MAR_THRESHOLD = 0.7
    EAR_CONSEC_FRAMES = 3


    blink_counter = 0
    long_blink_counter = 0
    yawn_counter = 0
    frame_below_thresh = 0
    yawn_active = False


    fatigue_score = 0
    score_update_interval = 30
    start_time = time.time()
    last_five_second_post = time.time()

    while True:
        ret, frame = stream.read()
        if not ret or frame is None:
            continue


        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        faces = face_detector(gray, 0)


        for face in faces:
            shape = landmark_predictor(gray, face)
            shape_np = face_utils.shape_to_np(shape)


            left_eye = shape_np[LEFT_EYE_INDICES]
            right_eye = shape_np[RIGHT_EYE_INDICES]
            mouth = shape_np[MOUTH_INDICES]


            left_ear = eye_aspect_ratio(left_eye)
            right_ear = eye_aspect_ratio(right_eye)
            ear = (left_ear + right_ear) / 2.0


            mar = mouth_aspect_ratio(mouth)


            # Blink logic
            if ear < EAR_THRESHOLD:
                frame_below_thresh += 1
            else:
                if frame_below_thresh >= EAR_CONSEC_FRAMES:
                    long_blink_counter += 1
                    print("Long blink detected")
                    send_event_message("Long blink detected")
                    #threading.Thread(target=send_event_message, args=("Long blink detected",), daemon=True).start()

                    
                elif frame_below_thresh > 0:
                    blink_counter += 1
                    print("Normal blink detected")
                    send_event_message("Normal blink detected")
                    #threading.Thread(target=send_event_message, args=("Normal blink detected",), daemon=True).start()

                frame_below_thresh = 0


            # Yawn logic
            if mar > MAR_THRESHOLD:
                if not yawn_active:
                    yawn_counter += 1
                    print("Yawn detected")
                    send_event_message("Yawn detected")

                    yawn_active = True
            else:
                yawn_active = False


            # Visualization
            cv2.drawContours(frame, [cv2.convexHull(left_eye)], -1, (0, 255, 0), 1)
            cv2.drawContours(frame, [cv2.convexHull(right_eye)], -1, (0, 255, 0), 1)
            cv2.putText(frame, f"EAR: {ear:.2f}", (30, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 255), 2)
            cv2.putText(frame, f"MAR: {mar:.2f}", (30, 60), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 0, 0), 2)


        # Score every minute
        if time.time() - start_time >= score_update_interval:
            fatigue_score = compute_fatigue_score(
                blinks=blink_counter,
                long_blinks=long_blink_counter,
                yawns=yawn_counter
            )
            print(f"Fatigue Score: {fatigue_score:.1f}")
            if fatigue_score >120:
                try:
                    requests.post(buzz)
                except Exception as e:
                    print("failed buzz")
            send_fatigue_score_to_esp32(fatigue_score)
            blink_counter = long_blink_counter = yawn_counter = 0
            start_time = time.time()

        
        cv2.imshow("Eye Aspect Ratio + Fatigue Score", frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break


    stream.release()
    cv2.destroyAllWindows()


if __name__ == "__main__":
    main()
