#include <ESP32Servo.h> // Library to Move the Servomotor

// Function prototypes
void serial_input();
void sonar_reset();
void serial_input();
String string_from_serial();
float get_distance();
float get_filtered_distance();
void print_distances();


Servo myServo; // Create servo object
const int servoPin = 1;  // Define servo pin = GPIO1
const int trigPin = 23;
const int echoPin = 18;
const int minUs = 500;    // Minimum servo pulse width in microseconds
const int maxUs = 2500;   // Maximum servo pulse width in microseconds

// Global variables
bool sonar_state = false;
int distances[16];

// Variables for non-blocking servo sweep
int servo_pos = 0;
int degrees_per_step = 12;
unsigned long last_servo_move = 0;
const int time_between_moves = 500; // Milliseconds between steps

// Hardcoded credentials
String sonar_name = "S1";
String sonar_pw = "demo123";

void setup() {
  Serial.begin(115200);
  ESP32PWM::allocateTimer(0);
  myServo.setPeriodHertz(50);  // Standard 50Hz servo
  myServo.attach(servoPin, minUs, maxUs);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  sonar_reset();
  Serial.println("");
}

void loop() {
  serial_input();


  // Only sweep if the sonar is active
  if (sonar_state) {
    if (millis() - last_servo_move >= time_between_moves) {
      last_servo_move = millis(); // Update the time of the last move
      
      int dist = (int)get_filtered_distance();
      int index = servo_pos / 12;
      if (index >= 0 && index < 16) distances[index] = dist;

      if (servo_pos == 0 || servo_pos == 180) {
        print_distances();
      }

      servo_pos += degrees_per_step;
      myServo.write(servo_pos);
      
      // Reverse direction at the ends of the sweep
      if (servo_pos >= 180 || servo_pos <= 0) {
        degrees_per_step *= -1; // Invert direction (1 becomes -1, -1 becomes 1)
      }
    }
  }
}



float get_distance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Timeout added (30ms) to prevent freezing if no echo is received
  long duration = pulseIn(echoPin, HIGH, 25000); 

  if (duration == 0) return 0; // No echo received
  
  return duration * 0.0343 / 2; //Returns the distance in centimeters
}

String string_from_serial() {
  if (Serial.available() > 0) {
    String user_input = Serial.readStringUntil('\n');
    user_input.trim();
    return user_input;
  }
  return ""; // Return empty string if there is no user input
}

void serial_input() {
  String command = string_from_serial();
  if (command.length() > 0) { // Process only if a command was received
    Serial.println(String(millis()) + " - Command: " + command);

    if (command == "start") {
      if (sonar_state) {
        Serial.println(">> SONAR ALREADY ON");
        return;
      }
      sonar_state = true;
      Serial.println(">> SONAR ACTIVE");
    } else if (command == "stop") {
      if (!sonar_state) {
        Serial.println(">> SONAR ALREADY OFF");
        return;
      }
      sonar_state = false;
      sonar_reset();
      Serial.println(">> SONAR TURNED OFF");
    }
    if (command == "distance") {
      Serial.println(get_filtered_distance());
    }
  }
}

void sonar_reset() {
    myServo.write(0);
    delay(200);
}

void print_distances() {
  Serial.print("Distances: ");
  for (int i = 0; i < 16; i++) {
    Serial.print(distances[i]);
    if (i < 15) Serial.print(", ");
  }
  Serial.print("\n");
}

float get_filtered_distance() {
  float readings[7];
  
  // Take 7 readings to mitigate noise
  for (int i = 0; i < 7; i++) {
    readings[i] = get_distance();
    // Short delay to ensure the previous ultrasonic pulse has faded
    // preventing interference between consecutive measurements.
    delay(30); 
  }

  // Simple bubble sort to organize readings
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6 - i; j++) {
      if (readings[j] > readings[j + 1]) {
        float t = readings[j];
        readings[j] = readings[j + 1];
        readings[j + 1] = t;
      }
    }
  }

  // Return the average of the 3 middle values (indices 2, 3, 4)
  return (readings[2] + readings[3] + readings[4]) / 3.0;
}