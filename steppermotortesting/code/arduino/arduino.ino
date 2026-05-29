#include <IFX9201_XMC1300_StepperMotor.h>

#define DIR_PIN IFX9201_STEPPERMOTOR_STD_DIR		// Pin 9 is standard DIR Pin
#define STP_PIN IFX9201_STEPPERMOTOR_STD_STP		// Pin 10 is standard STP Pin
#define DIS_PIN IFX9201_STEPPERMOTOR_STD_DIS		// Pin 11 is standard DIS Pin

const int StepsPerRevolution = 800;  // change this to fit the total number of steps per revolution for your motor

#if defined(XMC4700_Relax_Kit)
#define CONFIG_SERIAL Serial1
#else
#define CONFIG_SERIAL Serial
#endif


// Stepper motor object
Stepper_motor MyStepperMotor = Stepper_motor(StepsPerRevolution, DIR_PIN, STP_PIN, DIS_PIN);

IFX9201_STEPPERMOTOR_config_t example_config =
{
	.SteppingpMode = IFX9201_STEPPERMOTOR_STEPPINGMODE_FULL,
	.FreqPWMOut = 20000u,
	.PWMDutyCycleNormFactor = 10000u, 
	.NumMicrosteps = 64,
	.Store = IFX9201_STEPPERMOTOR_STEPPINGMODE_DO_NOT_STORE_CONFIG
};

IFX9201_STEPPERMOTOR_config_t example_config2 =
{
	.SteppingpMode = IFX9201_STEPPERMOTOR_STEPPINGMODE_HALF,
	.FreqPWMOut = 3000u,
	.PWMDutyCycleNormFactor = 5000u,
	.NumMicrosteps = 64,
	.Store = IFX9201_STEPPERMOTOR_STEPPINGMODE_DO_NOT_STORE_CONFIG
};

void setup() {
	// Signal Led
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);
  delay(2000);

  //config_driver();

  // set pins' mode as OUTPUT, set default speed and enable the stepper motor
  MyStepperMotor.begin();

  Serial.end();
  
  // set the speed at 10 rpm:
  MyStepperMotor.setSpeed(20);



  MyStepperMotor.step(-100);

  Serial.begin(115200);

}

bool config_driver(){
  IFX9201_STEPPERMOTOR_config_t config_read_back;

  // Apply config via serial interface
  MyStepperMotor.configure(CONFIG_SERIAL, &example_config);

  // Add a short delay to give the �C time to apply the config
  delay(100);

  // Read back written config
  MyStepperMotor.configRead(CONFIG_SERIAL, &config_read_back);

  if( (example_config.SteppingpMode == config_read_back.SteppingpMode) && \
       (example_config.FreqPWMOut == config_read_back.FreqPWMOut) && \
       (example_config.PWMDutyCycleNormFactor == config_read_back.PWMDutyCycleNormFactor) && \
       (example_config.NumMicrosteps == config_read_back.NumMicrosteps) && \
       (example_config.Store == config_read_back.Store) )
  {
    return true;
  }
  return false;
}


void loop() {

	
  digitalWrite(LED_BUILTIN, LOW);

	delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);

  delay(1000);

  if (Serial.available()){
        int number = Serial.parseInt();
        if (number != 0){
            Serial.println("read num");
            Serial.print(number);
        }
  }
  }
