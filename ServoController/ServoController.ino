#include <Servo.h>

Servo servo;

const int SERVO_PIN = 9;

// ── Tweakable Z boundaries ──────────────────────────────────────────────────
// Adjust these to match your sensor's actual observed range.
const float Z_MIN = -20.0;  // mT — minimum expected z value → maps to 0°
const float Z_MAX = 40.0;   // mT — maximum expected z value → maps to 180°

void setup() {
    Serial.begin(9600);
    servo.attach(SERVO_PIN);
    servo.write(90);
}

void loop() {
    if (!Serial.available()) return;

    // Expected serial format: "x,y,z\n" e.g. "1.23,-0.45,8.67"
    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) return;

    // Parse: skip x and y, extract z (third value)
    int firstComma = line.indexOf(',');
    if (firstComma < 0) return;
    int secondComma = line.indexOf(',', firstComma + 1);
    if (secondComma < 0) return;

    float z = line.substring(secondComma + 1).toFloat();

    // Linear mapping: Z_MIN → 0°, Z_MAX → 180°
    float angle = ((z - Z_MIN) / (Z_MAX - Z_MIN)) * 180.0;
    int angleClamped = constrain((int)angle, 0, 180);

    servo.write(angleClamped);
}
