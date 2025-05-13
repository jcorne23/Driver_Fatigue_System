# Driver Fatigue System - Weekly Progress Log
Week 1 Feb 10 - 14 | General Research and Proposal Writing
=========================================================

Sleep-deprived driving is an underappreciated but dangerously common problem. On a recent drive back home from Illinois in my Mustang GT, I experienced firsthand just how severe fatigue behind the wheel can become. Despite having lane assist and blasting music with caffeine in hand, I caught myself swerving repeatedly—my eyelids growing heavier, my focus slipping. There were stretches of road where my eyes were closed for what felt like forever, and I realized I was becoming a serious danger to myself and everyone around me. Rest stops felt untrustworthy, and I had no meaningful way to assess whether I was safe to continue driving.
This experience exposed a real and urgent gap in driver wellness awareness, especially when driving alone for long distances. What I truly needed wasn’t more coffee or loud music; I needed an objective, real-time system that could monitor my condition and help me make smarter decisions before it was too late.
This inspired the development of the Driver Fatigue System—a multi-modal platform designed to track signs of drowsiness, monitor facial fatigue cues like prolonged blinks or yawns, and even detect signs of intoxication. By combining computer vision, real-time feedback, and onboard alerts, this system aims to support drivers in staying safe, informed, and aware of their physical state while on the road.

![Image](https://github.com/user-attachments/assets/16e39c37-b5a6-4835-8183-b2b492563d60)


![Image](https://github.com/user-attachments/assets/08a9641c-2d15-45d8-98ee-b6bc5ac26e5d)


After identifying the dangers of drowsy driving from personal experience, we began designing a system that could monitor a driver’s alertness in real time. To do this, we needed a mix of physical sensors and wireless communication to assess wellness and deliver feedback quickly. The flowchart we developed became the blueprint for this vision, outlining how each component—camera, sensors, display, and servers—would connect and operate together.
At the heart of the system is the ESP32-S3, which collects sensor data and controls outputs like an OLED display, buzzer, and LED. Powered through a car’s USB port and regulated to 3.3V using an AMS1117, the ESP32 also connects to a DFRobot MQ-3 alcohol sensor and an OV2640 camera module. We initially planned to use a motion sensor (BNO055 IMU), but removed it after determining it was unnecessary.
To handle intensive tasks like image processing, we offloaded blink and yawn detection to a Raspberry Pi 4 running Python scripts and OpenCV, hosted on a Flask web server. Every 30 seconds, the Pi calculates a fatigue score and sends it back to the ESP32 or browser interface. A smartphone hotspot serves as the local network, assigning IPs to both the Pi and ESP32 for fast, local communication.
This system balances local device feedback with server-side intelligence, creating a responsive tool to help drivers assess their condition before it becomes dangerous, all sparked by one tired night on the road.

<img width="392" alt="Image" src="https://github.com/user-attachments/assets/d7d7afc0-b58b-4ffe-89ff-bc853396bab3" />

We begin to seek out parts from the ECE building and using our project funds to outsource on Amazon the items unavailable to us in the building. 

Week 2 Feb 17 - 21 | Project Proposal Review + Inital Design Stage
===================================================================

![Image](https://github.com/user-attachments/assets/3ce4ddd4-5eb7-42d0-ae61-9378f7b7e0e2)

This schematic represents our initial hardware mockup, developed during the early planning phase of the project. At this stage, we designed around an ESP32-DEVKITC-32E and prioritized including a USB-C connector for modern power delivery and convenience. Power from the USB-C port was stepped down to 3.3V using an AMS1117 regulator to supply the microcontroller and all peripherals. Key components such as the MQ-3 alcohol sensor, OLED display, LED, and buzzer were mapped out and connected directly to available GPIO pins. This early layout gave us full control over pin assignments and served as a foundational reference for system integration.


![Image](https://github.com/user-attachments/assets/0ea29115-624c-4cb9-b573-361633ed5ef6)

This image shows our initial PCB layout in KiCad, where we encountered several critical design oversights. One major issue was the USB-C port footprint, which was placed away from the board edge—making it physically unusable for cable connection. At this stage, we were still relying on a Molex-style connection scheme for the camera and sensors, which added wiring complexity. We used part libraries and footprint imports from Digi-Key to populate our schematic and layout quickly, but overlooked mechanical placement and enclosure fit. Despite its flaws, this version helped us become more familiar with PCB workflow and informed our corrected final revision.


During development, we realized the original camera configuration using Molex-style I2C connectors was not reliable or practical for repeated prototyping. This led us to explore alternative connection methods, ultimately choosing a camera strip connector similar to those used in commercial ESP32-CAM modules. The transition required sourcing a compatible connector and verifying pin alignment and signal integrity. This change greatly improved physical stability and reduced the chance of misalignment or connection failure. It also simplified integration with our PCB and reduced debugging time during testing.





Week 3 Feb 24 - 28 | Initial Design Completed
================================================

Initial Sensor Testing 


![Image](https://github.com/user-attachments/assets/27d98e17-5d49-470d-b80b-c4d68a93fda8)

We successfully got the MQ3 alcohol sensor working with the ESP32-DevKitC on a breadboard setup. By configuring GPIO 34 as an analog input and adjusting the resolution to 12 bits, we were able to read and convert raw ADC values into voltage. The live readings confirmed stable and responsive sensor output, validating our connection and code setup.






![Image](https://github.com/user-attachments/assets/7529c71d-385e-4b4e-8d8f-ee5ea69bb69d)


This OV2640 camera module was ultimately unusable in our system due to missing documentation and incomplete pin functionality. Notably, it lacks an exposed XCLK (external clock) pin, which is required to drive the camera sensor with a 10–20 MHz clock signal from the ESP32. Without this input, the module cannot be initialized or stream image data properly. Additionally, there were no available datasheets or verified user reviews to validate the pinout or confirm signal compatibility, making debugging nearly impossible. As a result, we replaced it with a more standardized ESP32-CAM board that included proper XCLK routing and a proven working configuration.


![Image](https://github.com/user-attachments/assets/a6ab00ae-fcef-4ce9-977f-ceaf0360d03b)



This schematic reflects the updated design in which we moved away from the ESP32-DevKitC and opted for a more compact and solder-friendly approach. We integrated a micro-USB port for easier power access and enclosure compatibility, using an AMS1117 regulator to supply 3.3V to the system. The selected ESP32 module connects via a flexible ribbon-style interface, simplifying the connection to the camera and reducing the number of manual jumper wires required. This change improved both assembly time and reliability, enabling a cleaner and more maintainable final hardware layout.













Week 4 Mar 3 - 7 | Teamwork Evaluation + Design Document + Inital Subsystem Implementation
========================================================================

![Image](https://github.com/user-attachments/assets/2bb636a5-7cf8-4ba8-b623-f4cee8a42069)




This is our revised PCB design, created after addressing the limitations of our initial prototype. In this version, we correctly positioned the micro-USB connector along the edge of the board to ensure full cable compatibility and structural alignment. The layout was reworked for improved functionality and cleaner trace routing to support components like the MQ-3 sensor, OLED display, buzzer, and LED indicator. This version also finalized mounting holes and better spacing to accommodate enclosure constraints and mechanical stability. Overall, the design reflects a more production-ready and functional board that integrates all project subsystems reliably.




![Image](https://github.com/user-attachments/assets/6215766a-dd48-430c-a006-83fe2e908f64)








To get I2C working on the ESP32-CAM, we had to manually assign pins since it doesn’t have dedicated I2C hardware lines. We selected GPIO 15 for SDA and GPIO 14 for SCL, then initialized a TwoWire object on bus 0. Using the Adafruit SSD1306 library with these custom-defined pins, we were able to successfully communicate with the OLED display. This setup allowed reliable screen output without interfering with the camera or WiFi, which often share key GPIOs on the ESP32-CAM.


Week 5 Mar 10 - 14 | Breadboard Demos
===================================================================


![Image](https://github.com/user-attachments/assets/e5a8b77d-b68d-4e6f-b4f6-03a9a45cf1b7)

A key breakthrough in our development came thanks to Vincent, who successfully installed the dlib library on the Raspberry Pi—a process known to be notoriously difficult due to its dependency on CMake and lengthy compile time. After having the blink and yawn detection logic running reliably on his local machine, Vincent worked through multiple build errors and optimized configurations to get dlib up and running on the Pi. Once installed, we were able to port over his ear_detection.py script, allowing real-time Eye Aspect Ratio (EAR) and Mouth Aspect Ratio (MAR) calculations to execute directly on the Pi. This milestone meant our fatigue detection could now operate independently on the embedded system without relying on external computing resources.


The code Vincent suggested for EAR detection based on landmarks


Week 6 Mar 17 - 21 | Spring Break
=================================================

![Image](https://github.com/user-attachments/assets/bf668777-70db-453f-a320-a6ffc568380f)

This image captures a crucial moment in our blink detection accuracy testing phase. Using OpenCV and dlib-based facial landmark tracking, we monitored Eye Aspect Ratio (EAR) values in real time to detect both normal and long blinks. The terminal output below logs each blink classification, allowing us to verify detection reliability visually and statistically. This setup was used extensively to fine-tune thresholds and confirm the system's ability to differentiate fatigue-related blink patterns. The annotated camera feed also helped us validate alignment and feature tracking frame-by-frame.







Week 7 Mar 24 - 28 | Research and Building Software
==================================================================================

Endpoint Routing ESP 32 SIDE

![Image](https://github.com/user-attachments/assets/a2decf1e-f466-4c3b-b56f-ff7a2d787c5d)

UI HANDLER 

POST Receiver 

![Image](https://github.com/user-attachments/assets/52a4e769-bde4-48b5-9adb-9a25cda0c260)


URI Registration 

![Image](https://github.com/user-attachments/assets/ebc02adc-a8c7-40e1-bc63-6a367c717c03)

The ESP32 server-side HTTP file defines and handles routes for POST-based control of hardware peripherals. In this example, the /flash endpoint is registered using httpd_uri_t, and tied to the flash_toggle_handler function. When a POST request is received, the handler parses the body to determine whether to turn the flashlight (GPIO 4) on, off, or set its brightness to a specific level using PWM. The flash_ui_handler also serves a simple HTML interface that allows manual control from any browser on the network. This implementation worked beautifully, providing low-latency feedback and making the ESP32 both network-controllable and UI-friendly.

![Image](https://github.com/user-attachments/assets/25722ac6-1897-4e2a-8725-4f47a0f0e30c)



During testing, we discovered that enabling WiFi using <WiFi.h> on the ESP32 interfered with analog pin readings, particularly on GPIO 15 used for the MQ3 alcohol sensor. To resolve this, we implemented a physical button that allows the user to disable WiFi before initiating the alcohol sensing routine. Once the button is pressed, the ESP32 disconnects from WiFi, shuts down the radio module, and begins the MQ3 test using analogRead without interference. This solution reliably restored ADC functionality and allowed us to balance network communication with sensor accuracy. It also gave us more control over when and how analog measurements were taken in the broader driver monitoring workflow.




UI 1.1 ESP32 SERVER 

![Image](https://github.com/user-attachments/assets/a4bc5aa5-5207-4e19-ac07-b04b63c23727)


This screenshot shows the original user interface hosted on the ESP32 web server, which was functional but lacked mobile responsiveness. The layout consisted of basic HTML with minimal styling, resulting in small, unaligned buttons and poor scaling on mobile devices. While the core features like flashlight control, MQ3 testing, and session launching worked as intended, the interface was difficult to use on smartphones. This early version served as a proof of concept but highlighted the need for better UI design and CSS styling for improved usability across devices.



![Image](https://github.com/user-attachments/assets/aa2577ff-9ac4-4524-8735-4cb4a0a4747a)




This image shows the improved and finalized version of our ESP32 server UI, optimized for mobile use. Compared to the original version, this interface features clearer button spacing, consistent alignment, and more intuitive styling for touchscreen interaction. Buttons are styled with hover effects, color cues, and padding that make each function, like starting a driving session or toggling the buzzer, easy to access on any smartphone. The layout responds well to different screen sizes and maintains readability, addressing the usability issues seen in our first prototype. This polished interface enhances the overall user experience and ties together the system’s core functions in a clean, professional presentation.

Week 8 Mar 31 - Apr 4 | 3rd Round PCB order
==================================================================================

Raspberry Pi Server Versions: 

![Image](https://github.com/user-attachments/assets/5c49aa2e-a5b7-49bd-ac0a-38d6b3af5f17)

In this Flask server implementation, we introduced Python's threading module to run the ear_detection.py script in a separate thread. This allowed the Raspberry Pi to continue handling incoming POST requests without being blocked by the long-running image processing task. By offloading the detection script to a background thread, we improved the server's responsiveness and ensured real-time data could still be received and processed concurrently. This was essential for maintaining smooth interaction between the ESP32 and the Pi during live monitoring.








![Image](https://github.com/user-attachments/assets/9746e6da-0d7a-4daa-948a-795ddfc2fe44)



This JavaScript code snippet powers the dynamic front-end behavior of our Raspberry Pi Flask server UI. When POST requests are received at the Pi server, incoming data is stored in arrays such as messages and fatigue_scores. These arrays are then passed to the HTML file, where functions like updateFeed() and updateChart() dynamically render the content using DOM manipulation and Chart.js, respectively. As a result, fatigue event messages appear in real time, and score trends are visualized without needing to refresh the page, creating a seamless, lifelike user experience.













Week 9 Apr 7 - 11 | Final PCB order
==================================================================================


![Image](https://github.com/user-attachments/assets/ed6b520b-8056-4276-9f8b-2693a01bb357)




This image shows our finalized prototype demo on a breadboard, featuring the ESP32-CAM connected with the MQ3 alcohol sensor, a button for triggering tests, and an active buzzer for alerts. All components are powered and wired to function together as a cohesive fatigue and BAC monitoring system. The OLED display is not shown here but is mounted inside the final enclosure. This setup was used to validate full system integration before soldering onto the custom PCB.





Week 10 Apr 14 - 18 | Continued Debugging and Research
==================================================================================


![Image](https://github.com/user-attachments/assets/76e24753-f7f9-412d-a7b1-f892c993fda1)



This interface serves as the real-time monitoring dashboard for our driver fatigue system. At the top, a scrolling terminal-style feed displays live detection messages such as "Normal blink detected" and "Long blink detected," streamed directly from the Raspberry Pi via POST requests. Below the feed, a line chart titled "Fatigue Score Over Time" visualizes the driver’s calculated fatigue levels, updating automatically in 30-second intervals. The fatigue score is computed based on blink frequency, duration, and other facial markers, and is plotted using Chart.js with a neon green aesthetic to match the terminal theme. Together, the feed and graph provide a responsive and informative snapshot of the driver’s alertness in real time.





Week 11 Apr 21 - 25 | MOCK DEMO
==================================================================================





![Image](https://github.com/user-attachments/assets/7ef69d92-0f8a-4d2c-8f49-70c35b7cf653)





This image shows one of our key testing sessions, where we evaluated communication latency and data flow across the system. The Raspberry Pi was used to run real-time computer vision algorithms while hosting a local Flask server. The top screen displays the live facial tracking with printed detection logs, while the laptop on the bottom shows the web interface receiving fatigue score updates over time. The ESP32-CAM and MQ3 alcohol sensor were connected via breadboard during this stage to simulate the final hardware behavior. These tests helped us tune the interval for message handling, validate our POST routing, and ensure the entire pipeline, from sensing to interface, responded with minimal delay.






To support multiple peripherals, including the MQ-3 alcohol sensor, an LED indicator, an OLED display, a button interface, and the ESP32-CAM running a WiFi server and camera stream, we initially powered the system with a 5V 1A supply. However, the system exhibited instability during peak load conditions, particularly when both the WiFi module and camera were active, occasionally causing brownouts and spontaneous resets. To address this, we upgraded to a 5V 2.4A regulated source. The estimated current draw was calculated as follows:

Week 12 Apr 28 - May 2 | DEMO WEEK
==================================================================================


![Image](https://github.com/user-attachments/assets/e95765fd-96d7-4ca6-9305-65f228d4e0a5)

![Image](https://github.com/user-attachments/assets/d56c35d6-3ae8-4889-9f77-4430f9e00510)

This image shows the final PCB implementation with all soldered components and jumper wires securely in place. The ESP32-CAM module is mounted directly onto the board, interfacing cleanly with the underlying traces connected to the OLED, MQ3 sensor, LED, and buzzer. Unlike earlier prototypes that relied heavily on breadboards and loose wiring, this design offers improved electrical stability and structural integrity. The soldered connections ensure robust signal delivery across all subsystems while maintaining a clean and modular layout. This finalized PCB reflects the culmination of our hardware integration efforts and is fully functional as tested with the completed driver monitoring system.


Final Enclosure ready for final demo! Everything is working! 


![Image](https://github.com/user-attachments/assets/cc3eed5d-efd2-4101-85de-45a522fb4333)


Week 13 May 5 - 9 | FINAL WEEK WRAPPING UP
==================================================================================

In this week, we made finishing touches to our Final Presentation Slides  
Successfully presented our project to Professor, TAs, and peers 
