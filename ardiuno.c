#include <Servo.h>

Servo myServo;

#define servo A5

// ...existing code...

void setup() {
  Serial.begin(9600);

  pinMode(R_S, INPUT);
  pinMode(L_S, INPUT);

  pinMode(echo, INPUT);
  pinMode(trigger, OUTPUT);

  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enB, OUTPUT);

  analogWrite(enA, motorSpeed);
  analogWrite(enB, motorSpeed);

  // remove pinMode(servo, OUTPUT); // Servo library manages pin
  myServo.attach(servo);

  centerServo();
  delay(300);
  distance_F = Ultrasonic_read();
}

// replace servoPulse and centerServo with Servo library usage
void servoPulse(int pin, int angle) {
  // now uses Servo write (pin parameter unused, kept for compatibility)
  myServo.write(constrain(angle, 0, 180));
  delay(20); // small settle time
}

void centerServo() {
  myServo.write(90); // use 90 as center; calibrate if needed
  delay(200);
}

// add a quick side scan function
void scanSides() {
  // look right first (adjust angles to your servo orientation)
  myServo.write(20); // right
  delay(150);
  distance_R = Ultrasonic_read();

  // look left
  myServo.write(160); // left
  delay(150);
  distance_L = Ultrasonic_read();

  // center again
  centerServo();
}

// ...existing code...

void loop() {
  distance_F = Ultrasonic_read();
  Serial.print("Front: "); Serial.println(distance_F);

  int leftSensor = digitalRead(L_S);
  int rightSensor = digitalRead(R_S);

  // ===== Line Following + Obstacle Avoidance =====
  if (leftSensor == 0 && rightSensor == 0) {
    // Both sensors see white → move forward unless obstacle
    if (distance_F > Set) {
      // scan sides while moving forward so side obstacles are detected
      scanSides();
      Serial.print("Left: "); Serial.print(distance_L);
      Serial.print("  Right: "); Serial.println(distance_R);

      // if either side is too close, choose an avoidance path
      if (distance_L <= Set || distance_R <= Set) {
        chooseBestPath();
      } else {
        forward();
      }
    } else {
      chooseBestPath();
    }
  }
  else if (rightSensor == 1 && leftSensor == 0) {
    turnRight();
  }
  else if (rightSensor == 0 && leftSensor == 1) {
    turnLeft();
  }
  else {
    // Fail-safe: unexpected readings → go forward
    forward();
  }

  delay(10);
}

// update chooseBestPath to use the same angles (already scanned maybe)
void chooseBestPath() {
  Stop();
  delay(100);

  // if distances not recent, do a scan
  scanSides();

  if (distance_L > Set || distance_R > Set) {
    if (distance_L > distance_R) {
      turnLeft();
      delay(400);
    } else {
      turnRight();
      delay(400);
    }
  } else {
    // Both blocked → reverse and turn
    backword();
    delay(400);
    turnRight();
    delay(500);
  }
}