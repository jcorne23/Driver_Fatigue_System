
#include "esp_camera.h"
#include <WiFi.h>
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>



#define CAMERA_MODEL_ESP32_CAM_BOARD
#define MQ3_PIN  15     // PIN FOR THE MQ3
#define LED_FLASH 4     // PIN FOR THE FLASHLIGHT 
#define MQ3_LED_PIN 13  // GPIO 15 controls the external LED (if safe)
#define BUTTON_PIN 2

//OLED DEFINES
#define I2C_SDA 16
#define I2C_SCL 14
TwoWire I2Cbus = TwoWire(0);

// Display defines
#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   64
#define OLED_RESET      -1
#define SCREEN_ADDRESS  0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &I2Cbus, OLED_RESET);

bool mq3TestStarted = false;  


#include "camera_pins.h"

// ============================
// Enter your WiFi credentials
// ===========================
const char *ssid =  "NETGEAR97";  
const char *password = "slowunit520";

void startCameraServer();

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  Serial.println();
  analogReadResolution(12);
  ledcAttach(4, 5000,8);   // Attach GPIO 4 to channel
  ledcWrite(4, 0);  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW); // Buzzer starts OFF

  I2Cbus.begin(I2C_SDA, I2C_SCL, 100000);
  Serial.println("Initialize display");

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.printf("SSD1306 OLED display failed to initalize.\nCheck that display SDA is connected to pin %d and SCL connected to pin %d\n", I2C_SDA, I2C_SCL);
    while (true);
  }
  Serial.println("Show 'Hello World!' on display");


  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;  
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  if (config.pixel_format == PIXFORMAT_JPEG) {
    if (psramFound()) {
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);        // flip it back
    s->set_brightness(s, 1);   // up the brightness just a bit
    s->set_saturation(s, -2);  // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  if (config.pixel_format == PIXFORMAT_JPEG) {
    s->set_framesize(s, FRAMESIZE_QVGA);
  }

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

#if defined(CAMERA_MODEL_ESP32S3_EYE)
  s->set_vflip(s, 1);
#endif

// Setup LED FLash if LED pin is defined in camera_pins.h
#if defined(LED_GPIO_NUM)
  setupLedFlash(LED_GPIO_NUM);
#endif

  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");




display.clearDisplay();

// Big header
display.setTextSize(2);
display.setTextColor(SSD1306_WHITE);
display.setCursor(0, 0);
display.println("Driver");

display.setCursor(0, 16);
display.println("Fatigue");

display.setCursor(0, 32);
display.println("System");

// Small IP at bottom
display.setTextSize(1);
display.setCursor(0, 56);
display.print("IP: ");
display.print(WiFi.localIP());

display.display();  // Push everything at once

}



void runMQ3Test() {
  pinMode(MQ3_LED_PIN, OUTPUT);

  // Warmup countdown
  for (int i = 1; i <= 15; i++) {
    display.clearDisplay();
    display.setTextSize(2);
    const char* title = "BAC TEST";
    int16_t titleWidth = strlen(title) * 12;
    int16_t titleX = (128 - titleWidth) / 2;
    display.setCursor(titleX, 0);
    display.setTextColor(SSD1306_WHITE);
    display.println(title);

    display.setTextSize(1);
    String warmupStr = "Warmup: " + String(i) + " / 15 sec";
    int16_t warmupWidth = warmupStr.length() * 6;
    int16_t warmupX = (128 - warmupWidth) / 2;
    display.setCursor(warmupX, 36);
    display.println(warmupStr);

    display.display();
    delay(1000);
  }
  digitalWrite(MQ3_LED_PIN, HIGH); 

  // Measurement loop
  for (int i = 0; i < 30; i++) {
    int rawValue = analogRead(MQ3_PIN);
    float voltage = rawValue * (3.3 / 4095.0);

    // Calculate BAC
    float bac = 0.0;
    if (voltage > 1.8) {
      bac = (voltage - 1.8) * (0.20 / (3.3 - 1.8));
      // Optional: Clamp maximum BAC at 0.20
      if (bac > 0.20) bac = 0.20;
    } else {
      bac = 0.0;
    }

    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.println("MQ-3 Sensor");
    display.print("Raw: ");
    display.println(rawValue);
    display.print("Voltage: ");
    display.print(voltage, 3);
    display.println(" V");

    display.print("BAC: ");
    display.print(bac, 3);  // 3 decimal places
    display.println(" %");

    display.setCursor(0, 56);
    display.print("Sec: ");
    display.print(i + 1);
    display.print(" / 30");

    display.display();
    delay(1000);
  }


  // Final message
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("MQ3 Test Done");
  display.display();

  digitalWrite(MQ3_LED_PIN, LOW);
}



void loop() {
  if (!mq3TestStarted && digitalRead(BUTTON_PIN) == LOW) {
    delay(50);
    if (digitalRead(BUTTON_PIN) == LOW) {  // Check again after debounce
      Serial.println("Button pressed! Starting MQ3 Test.");

      mq3TestStarted = true;  // Mark that the MQ3 test is now running

      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);
      Serial.println("WiFi disconnected.");

      
      runMQ3Test();

      while (true) {
        delay(1000);  // Just freeze here for now (safe)
      }
    }
  }

  delay(10);  // Tiny delay to make the loop CPU-friendly
}