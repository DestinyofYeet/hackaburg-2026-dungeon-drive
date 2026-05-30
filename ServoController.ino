#include <Servo.h>

Servo servo;

const int SERVO_PIN = 9; // PWM pin on Arduino Uno

void setup() {
    Serial.begin(115200);
    servo.attach(SERVO_PIN);
    servo.write(90); // centre on startup
    Serial.println("ServoController ready. Send: servo: <0-180>");
}

void loop() {
    if (!Serial.available()) return;

    String line = Serial.readStringUntil('\n');
    line.trim();

    // Expected format: "servo: 90"
    if (!line.startsWith("servo: ")) return;

    int angle = line.substring(7).toInt();
    angle = constrain(angle, 0, 180);
    servo.write(angle);

    Serial.print("OK: ");
    Serial.println(angle);
}
