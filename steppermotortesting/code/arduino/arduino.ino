#include <IFX9201_XMC1300_StepperMotor.h>
#include <Servo.h>

#define DIR_PIN_X 9  // Pin 9 is standard DIR Pin
#define STP_PIN_X 10 // Pin 10 is standard STP Pin
#define DIS_PIN_X 7  // Pin 11 is standard DIS Pin
#define DIR_PIN_Y 3  // Pin 9 is standard DIR Pin
#define STP_PIN_Y 5  // Pin 10 is standard STP Pin
#define DIS_PIN_Y 8  // Pin 11 is standard DIS Pin
const int SERVO_PIN = 6;
const float Z_MIN = -20.0; // mT — minimum expected z value → maps to 0°
const float Z_MAX = 40.0;  // mT — maximum expected z value → maps to 180°

const int StepsPerRevolution = 800; // change this to fit the total number of
                                    // steps per revolution for your motor

#if defined(XMC4700_Relax_Kit)
#define CONFIG_SERIAL Serial1
#else
#define CONFIG_SERIAL Serial
#endif

// Stepper motor object
Stepper_motor M_X =
    Stepper_motor(StepsPerRevolution, DIR_PIN_X, STP_PIN_X, DIS_PIN_X);
Stepper_motor M_Y =
    Stepper_motor(StepsPerRevolution, DIR_PIN_Y, STP_PIN_Y, DIS_PIN_Y);

Servo servo;

void setup() {
  // Signal Led
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  delay(2000);

  // set pins' mode as OUTPUT, set default speed and enable the stepper motor
  M_X.begin();

  // set the speed at 10 rpm:
  M_X.setSpeed(300);

  M_Y.begin();
  M_Y.setSpeed(300);
  servo.attach(SERVO_PIN);
  servo.write(90);

  Serial.begin(115200);
}

void step(int16_t x, int16_t y) {
  uint32_t last_step_time = 0;
  float duty_ratio = 0.5;
  uint32_t step_delay = 296;
  ;
  M_X.update_accumulated_steps(x);
  M_Y.update_accumulated_steps(y);

  Serial.print("step_delay");
  Serial.print(step_delay);

  uint16_t direction_x = 0;
  if (x > 0) {
    direction_x = 1;
  }
  uint16_t direction_y = 0;
  if (y > 0) {
    direction_y = 1;
  }
  digitalWrite(DIR_PIN_X, direction_x);
  digitalWrite(DIR_PIN_Y, direction_y);
  x = abs(x);
  y = abs(y);
  while (x > 0 || y > 0) {
    uint32_t now = micros();

    while (now - last_step_time < (duty_ratio * step_delay)) {
      now = micros();
    }

    // get the timeStamp of when the motor half stepped:
    last_step_time = now;

    // output a HIGH half step to STP pin
    if (x > 0) {
      digitalWrite(STP_PIN_X, HIGH);
    }
    if (y > 0) {
      digitalWrite(STP_PIN_Y, HIGH);
    }

    while (now - last_step_time < (duty_ratio * step_delay)) {
      now = micros();
    }

    // get the timeStamp of when the motor half stepped:
    last_step_time = now;

    // output a LOW half step to STP pin
    if (x > 0) {
      digitalWrite(STP_PIN_X, LOW);
      x--;
    }
    if (y > 0) {
      digitalWrite(STP_PIN_Y, LOW);
      y--;
    }
  }
}
void loop() {
  int x, y, z;

  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');

    if (sscanf(line.c_str(), "%d,%d,%d", &x, &y, &z) == 3) {
      Serial.print("x=");
      Serial.println(x);
      Serial.print("y=");
      Serial.println(y);
      Serial.print("z=");
      Serial.println(z);
      if (x != 0 || y != 0) {
        Serial.print("x: ");
        Serial.print(x);
        Serial.print(" ");
        Serial.print("y: ");
        Serial.print(y);
        Serial.print(" ");
        step(x, y);
      }
      if (z != 0) {
        Serial.print("moving servo");
        Serial.print(z);
        Serial.print(" ");
        float angle = ((z - Z_MIN) / (Z_MAX - Z_MIN)) * 180.0;
        int angleClamped = constrain((int)angle, 0, 180);

        servo.write(angleClamped);
      }
    }
  }
}
