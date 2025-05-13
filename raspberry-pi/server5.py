import subprocess
import threading
from flask import Flask, request, render_template, jsonify
import time

app = Flask(__name__)

messages = []
fatigue_scores = []

def start_ear_detection():
    subprocess.Popen(['python3', 'ear_detection.py'])

@app.route('/startdrive', methods=['POST'])
def start_drive():
    print("POST received: Starting ear_detection.py...")
    try:
        threading.Thread(target=start_ear_detection).start()
        return "Driving session started", 200
    except Exception as e:
        return f"Failed to start: {str(e)}", 500

emergency_triggered = False  # Global flag


@app.route('/report', methods=['POST'])
def report_data():
    global emergency_triggered
    data = request.get_json()
    #print(f"Received report: {data}")


    if "score" in data:
        fatigue_scores.append({
            'time': time.strftime('%H:%M:%S'),
            'score': data['score']
        })


        if data['score'] >= 120:
            emergency_triggered = True


    if "message" in data:
        messages.append(data.get("message", ""))


    return "Report received", 200


@app.route('/')
def index():
    return render_template('index.html')

@app.route('/data')
def get_data():
    return jsonify({
        'messages': messages,
        'fatigue_scores': fatigue_scores
    })



@app.route('/emergency')
def emergency():
    global emergency_triggered
    if emergency_triggered:
        emergency_triggered = False
        # This opens the phone dialer automatically when page is loaded
        return '''
        <html>
          <head>
            <meta http-equiv="refresh" content="0; url=tel:+15551234567" />
          </head>
          <body>
            <p>If the call did not start, <a href="tel:+15551234567">click here to call</a>.</p>
          </body>
        </html>
        '''
    else:
        return "No emergency detected."

@app.route('/check_emergency')
def check_emergency():
    global emergency_triggered
    return jsonify({'emergency': emergency_triggered})


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
