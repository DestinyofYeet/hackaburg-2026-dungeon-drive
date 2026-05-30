#include <IFX9201_XMC1300_StepperMotor.h>

#define DIR_PIN_X 9		// Pin 9 is standard DIR Pin
#define STP_PIN_X 10		// Pin 10 is standard STP Pin
#define DIS_PIN_X 7		// Pin 11 is standard DIS Pin
#define DIR_PIN_Y 3    // Pin 9 is standard DIR Pin
#define STP_PIN_Y 5    // Pin 10 is standard STP Pin
#define DIS_PIN_Y 8    // Pin 11 is standard DIS Pin

const int StepsPerRevolution = 800;  // change this to fit the total number of steps per revolution for your motor

#if defined(XMC4700_Relax_Kit)
#define CONFIG_SERIAL Serial1
#else
#define CONFIG_SERIAL Serial
#endif


// Stepper motor object
Stepper_motor M_X = Stepper_motor(StepsPerRevolution, DIR_PIN_X, STP_PIN_X, DIS_PIN_X);
Stepper_motor M_Y = Stepper_motor(StepsPerRevolution, DIR_PIN_Y, STP_PIN_Y, DIS_PIN_Y);


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


  Serial.begin(115200);

}


void loop() {
  int x, y, z;

  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');

    if (sscanf(line.c_str(), "%d,%d,%d", &x, &y, &z) == 3) {
      Serial.print("x="); Serial.println(x);
      Serial.print("y="); Serial.println(y);
      Serial.print("z="); Serial.println(z);
      if ( x != 0){
        Serial.print("x: ");
        Serial.print(x);
        Serial.print(" ");
        M_X.step(x);
      }
      if ( y != 0){
        Serial.print("y: ");
        Serial.print(y);
        Serial.print(" ");
        M_Y.step(y);
      }
      if ( z != 0){
        Serial.print("moving servo");
        Serial.print(z);
        Serial.print(" ");
      }
    }
  }

  
}
