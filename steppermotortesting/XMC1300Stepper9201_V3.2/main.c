/*
 * main.c
 *
 *  Date of last modification  14.09.2017
 *  Version V4.0
 *  Author: Achim Schuette
 */




/*
ACMPs
ACMP InN    InP
0    P2.8   P2.9		Current detection sin
1    P2.6   P2.7		Current detection cos
2    P2.2   P2.1
*/

#include <DAVE.h>                 //Declarations from DAVE Code Generation (includes SFR declaration)

#include "probe_scope.h"
#include "XMC1_SCU.h"

//--------------------------------------------------------------
// user includes
//--------------------------------------------------------------
#include <stdint.h>
#include <xmc1300.h>
#include <math.h>

//--------------------------------------------------------------
// global user types
//--------------------------------------------------------------
typedef struct _MicroStepSwitchPoint
{
	uint16_t u16Sin;
	uint16_t u16Cos;
	uint16_t u16SinMod;
	uint16_t u16CosMod;
	uint32_t u16DirCtrl_Sin;
	uint32_t u16DirCtrl_Cos;
}T_MicroStepSwitchPoint;

typedef struct _HalfFullSwitchPoint
{
	uint32_t u16DirCtrl_Sin;
	uint32_t u16PWMCtrlON_Sin;
	uint32_t u16DirCtrl_Cos;
	uint32_t u16PWMCtrlON_Cos;
}T_HalfFullSwitchPoint;

/*
typedef struct _ADCResults
{
	uint16_t u16_ISin;
	uint16_t u16_ICos;
	uint16_t u16_Udc;
	uint16_t u16_Poti;
	uint32_t u32SampleTick;
}T_ADCResults;
*/
typedef struct _ADCResults
{
	uint16_t u16_K1;
	uint16_t u16_K2;
}T_ADCResults;


typedef struct _T_RxData
{
	uint8_t au8ReceiveDataAll[32];
	uint8_t au8ReceiveData[28];
    uint8_t u8NumOfRxBlocks;
    uint8_t u8RemainingRxData;
	uint8_t u8DataMode;
	uint8_t u8RxCounter;

}T_RxDataFromPC;

typedef struct _T_TxData
{
	uint8_t au8SendDataAll[32];
	union
	{
	 uint16_t au16SendData[14];
	 uint8_t au8SendData[28];
	};
	uint8_t u8NumOfTxBlocks;
    uint8_t u8RemainingTxData;
	uint8_t u8DataMode;
	uint8_t u8TxCounter;
}T_TxDataToPC;


typedef struct _T_MotorData
{
	uint16_t u16MotorType;
	uint16_t u16StepsPerRef;
	uint16_t u16MotorSpeed;
	uint16_t u16MotorCurrent;
	uint8_t  u8DataStatus;
}T_MotorData;

typedef struct _T_BoardData
{
	uint16_t u16_f_IrefSin;
	uint16_t u16_f_IrefCos;
	uint16_t u16MinOnTimeSin;
	uint16_t u16MinOnTimeCos;
	uint16_t u16FixOffTimeSin;
	uint16_t u16FixOffTimeCos;
	uint16_t u16_f_ChopSin;
	uint16_t u16_f_ChopCos;
	uint8_t  u8DataStatus;
}T_BoardData;

typedef struct _T_StepData
{
	uint16_t u16WaveFormNo;
	uint16_t u16NumOfMicroSteps;
	uint16_t u16OverCurrentFactor;
	uint8_t  u8Direction;
	uint8_t  u8DataStatus;
}T_StepData;

typedef struct _T_ControlData
{
	uint16_t u16MagicWord;
	uint8_t  u8EnableMotor;
	uint8_t  u8free1;
	uint8_t  u8DataStatus;
}T_ControlData;

typedef struct _T_ScopeData
{
	uint8_t  u8Ch1SignalIdx;
	uint8_t  u8Ch2SignalIdx;
	uint8_t  u8NumOfSamplesIdx;
	uint8_t  u8SampleTimeIdx;
	uint8_t  u8Control;
	uint8_t  u8DataStatus;
}T_ScopeData;



//--------------------------------------------------------------
// Global Defines
//--------------------------------------------------------------
//Current Measurement  not used so far
#define VRefADC_V			    5.000	//5V ADC Reference Voltage
#define ADCResolution12bit_mV	1.220	//5V / 4096 = 1220uV
#define InvCurrentAmplGain_k	4.185	//OP Amplifier
#define ShuntResistor_mOhm	    1000.0  //10kOhm

//#define pi  				3.14159265  //not used so far
#define pi_2  				1.57079632



//--------------------------------------------------------------
// global user const data
//--------------------------------------------------------------
/* ChipID auslesen
uint32_t *CSIDptr;
uint32_t ID_Value;
uint32_t ID_Value_Adr = 0x10000FE8;

CSIDptr = ID_Value_Adr;

ID_Value = *CSIDptr;

CHIP ID
0001 4XXXH XMC1400
0001 XXX2H temperature : -40 - 85° C
0001 XXX3H temperature : -40 - 105° C
0001 XX4XH VQFN40 pin package
0001 XX8XH VQFN48 pin package
0001 XX9XH VQFN64 pin package
0001 XXAXH LQFP64 pin package
Others Reserved

*/


/*High Current Out
  P1.0,P1.1,P1.2,P1.3,P1.4,P1.5
  Switching Table for Fullstep and Halfstep
  Port1 .5,.4,.2,.0
  Gates  ---- ---- --DC -A-B
  Port1  0000 0000 0011 0101
  Gate High = HighSide ON, One halfbridge is controlled by one gate only
  i.e.: P1.2 controls A and A#  and 0.2 controls B and B#
  Note P1.2 and P0.2 as well as P1.4 and P0.12 are linked together!
  Due to this fact diagonal control of the halfbridge is controlled via port config register!
  Sin-Load current flow from A via RL-Load to B# is defined to be positive dto. for Cos
  Topology Phase-Sin   VCC
 			High:	A		B
		 	Load:	   -RL-
			 Low: 	A#		B#
      GateControl--P0.7----P0.2
		 	 	 	   GND

  Topology Phase-Cos   VCC
 			High:	C		D
		 	Load:	   -RL-
			 Low: 	C#		D#
      GateControl--P0.8----P0.12
		 	 	 	   GND
*/



  //start SYSTisk
  //PPB->SYST_RVR = 0x00FFFFFF;    // Systimer auf maxwert
  //PPB->SYST_CSR = 5;             //Systimer starten
  //u32LatchStart = PPB->SYST_CVR; //Systimer startwert latchen
  //atLaufzeiten[0].u32StartWert = PPB->SYST_CVR; //Systimer startwert latchen


//0x1U:     set P0.12 to high => L/R-Dir Control =1 => pos Current in SIN coil
//0x10000U: set P0.12 to low  => L/R-Dir Control =1 => neg Current in SIN coil
//PWM:      0x80:OFF config as digout (was never set), 0xA8:ON config as CCU8out via ALT-Function 5

//0x1U:     set P0.8 to high  => L/R-Dir Control =1 => pos Current in COS coil
//0x10000U: set P0.8 to low   => L/R-Dir Control =1 => neg Current in COS coil
//PWM:      0x80:OFF config as digout (was never set), 0xA8:ON config as CCU8out via ALT-Function 5

const T_HalfFullSwitchPoint FullStepSwitching[4] =
{//DIRsin,PWMsin,DIRcos,PWMCos
 {0x1U    ,0xA8U,0x1U    ,0x80U},	    //DIRsin:+ PWMsin:ON , DIRcos:+, PWMcos:OFF
 {0x1U    ,0x80U,0x1U    ,0xA8U},	    //DIRsin:+ PWMsin:OFF, DIRcos:+, PWMcos:ON
 {0x10000U,0xA8U,0x10000U,0x80U},	    //DIRsin:- PWMsin:ON , DIRcos:-, PWMcos:OFF
 {0x10000U,0x80U,0x10000U,0xA8U}        //DIRsin:- PWMsin:OFF, DIRcos:-, PWMcos:ON
};

const T_HalfFullSwitchPoint HalfStepSwitching[8] =
{//DIRsin,PWMsin,DIRcos,PWMCos
 {0x1U    ,0xA8U,0x1U    ,0x80U},	    //DIRsin:+ PWMsin:ON , DIRcos:+, PWMcos:OFF  sin+
 {0x1U    ,0xA8U,0x1U    ,0xA8U},	    //DIRsin:+ PWMsin:ON , DIRcos:+, PWMcos:ON   sin+ cos+

 {0x10000U,0x80U,0x1U    ,0xA8U},	    //DIRsin:- PWMsin:OFF, DIRcos:+, PWMcos:ON   cos+
 {0x10000U,0xA8U,0x1U    ,0xA8U},       //DIRsin:- PWMsin:ON , DIRcos:+, PWMcos:ON   cos+ sin-

 {0x10000U,0xA8U,0x10000U,0x80U},	    //DIRsin:- PWMsin:ON , DIRcos:-, PWMcos:OFF  sin-
 {0x10000U,0xA8U,0x10000U,0xA8U},	    //DIRsin:- PWMsin:ON , DIRcos:-, PWMcos:ON   sin- cos-

 {0x1U    ,0x80U,0x10000U,0xA8U},	    //DIRsin:+ PWMsin:OFF, DIRcos:-, PWMcos:ON   cos-
 {0x1U    ,0xA8U,0x10000U,0xA8U}        //DIRsin:+ PWMsin:ON , DIRcos:-, PWMcos:ON   cos- sin+
};


//--------------------------------------------------------------
// global user variables
//--------------------------------------------------------------
// Motordata
// float Imaxmot_mA = 300;             //max Motor Current in mA 200... 1000, 2000,  10000 - not used, see u32PWM_DutyCycleNormFactor
                                    // not used so far
uint16_t NumOfMicroSteps    = 32;   // 32.. 128 Number of Steps in SinCos Table representing 360 degree
uint32_t u32StepFerquency   = 1000; // Frequency of internal Test-Stepgenerator
uint16_t u16NumOfStepsMotor = 200;  // Steps per Revolution for internal Test-Stepgenerator
uint16_t u16Revolution      = 10;	// Num of Revolutions for internal automated Demo via Test-Stepgenerator

float f32Trimfaktor = 3333;					// current limit for Microstep calculation. 10000 = 100% = 5V or 3A
uint32_t u32PWM_DutyCycleNormFactor = 3333;  //10000 = 100% = 5V or 3A max current limit. Only for full and half step.

volatile uint32_t u32PWM_DutyCycle_Sin;     // DutyCycle SEetpoint for Current CompareValues sin-Coil
volatile uint32_t u32PWM_DutyCycle_Cos;     // DutyCycle SEetpoint for Current CompareValues cos-Coil
volatile uint32_t u32CompareThreshold = 50;  // Noise threshold for Current CompareValue in order to guaranty correct CMP behavior at zero crossing

volatile uint32_t u32FreqGateTime_Sin;	//Gate-Timer DC is fix 100%, 100% of 50kHz = 20us
volatile uint32_t u32FreqGateTime_Cos;  //Gate-Timer DC is fix 100%, 100% of 50kHz = 20us

volatile uint32_t u32NoiseGatingTime_Sin_us = 20;      //20us Gating for sinCurrent Compare due to noise blanking
volatile uint32_t u32NoiseGatingTime_Cos_us = 20;      //20us Gating for cosCurrent Compare due to noise blanking


volatile uint32_t u32FreqSinGen_Hz = 32000;	//Frequency of PWM-Sin Generator 100000 = 100kHz
volatile uint32_t u32FreqCosGen_Hz = 32000;    //Frequency of PWM-Cos Generator 100000 = 100kHz

volatile uint32_t u32FreqPWMOutSin_Hz = 3000;  // 3kHz switching Frequency for PWM generation sin
volatile uint32_t u32FreqPWMOutCos_Hz = 3000;  // 3kHz switching Frequency for PWM generation cos

volatile uint32_t u32DCPWMOutSin   = 5000;  // 50% duty cycle for PWM generation sin
volatile uint32_t u32DCPWMOutCos   = 5000;  // 50% duty cycle for PWM generation cos


volatile uint32_t u32PWM_DutyCycle_FullHalf_Sin;
volatile uint32_t u32PWM_DutyCycle_FullHalf_Cos;

//CPU
volatile uint16_t u16DieTemp_C;
volatile uint32_t u32DieTemp_K;

//Counter
volatile uint32_t u32MainCounter;
volatile uint32_t u16ISRStepCounter;
volatile uint32_t u32ISRCounter_10_sec;

//Timer
volatile uint32_t u32SysTimer_ms;
volatile uint32_t u32ISRTimer_ms;
//volatile uint32_t u32SysTimerValues[16];

//dig IOs

T_TxDataToPC   TxData_ToPC;
T_RxDataFromPC RxData_FromPC;
UART_STATUS_t  Uart_RxState;
UART_STATUS_t  Uart_TxState;

T_MotorData   MotorData;
T_BoardData   BoardData;
T_StepData    StepData;
T_ControlData ControlData;
T_ScopeData   ScopeData;

volatile uint32_t  u32TxFinishedCounter;
volatile uint32_t  u32RxFinishedCounter;
volatile uint32_t  u32RxFinishedCounterMain;

//ADC
volatile uint16_t  u16ADCResult_Poti_akt;
volatile uint16_t  u16ADCResult_Udc_akt;
volatile uint16_t  u16ADCResult_Isin_akt;
volatile uint16_t  u16ADCResult_Icos_akt;

volatile uint16_t  u16ADCResult_Poti_akt_mV;
volatile uint16_t  u16ADCResult_Udc_akt_V;
volatile uint16_t  u16ADCResult_Isin_akt_mA;
volatile uint16_t  u16ADCResult_Icos_akt_mA;

volatile T_ADCResults au16ADC_Results[1200];
volatile uint16_t  u16LatchIndex = 0;           // indexcounter for ADC Result capture

//sin cos tab init
T_MicroStepSwitchPoint atMicroStepSinCosTable[128+1];  //switch pattern table
float fAngle;		  // Angle for Sin / Cos switching pattern initialization
float fDeltaAngle;    // AngleDifferenz for Sin / Cos switching pattern initialization

//Supervisor Control
int16_t s16Index = 0;         // Indexcounter for accessing switching pattern table
uint8_t bEnable = 0;          // Enable Power
uint8_t bDirection = 0;       // MotorShaft - clocking direction 0: CW;  1: CCW

uint8_t u8STMState = 0;
uint8_t u8SteppingMode = 1; /*0: OFF
                              1: Fullstep,
                              2: HalfStep,
                              3: Quarter Step (not implemented so far)
                              4: Microstep sin cos
                              5: Microstep sin cos overlapping (not implemented so far)
                              6: Microstep sin cos modified
                              */

uint8_t u8ControlMode = 1; /* 0: OFF
                              1: External Dir - Step
                              2: DemoMode
                              3: Manual via COM (not implemented so far)
                              4: Service Mode   (not implemented so far)
                           */


//volatile uint8_t u8ForceSinCosRefValues = 0;
//volatile uint8_t u8PowerEnable          = 0;
//volatile uint8_t u8SetSinDirPositive    = 0;
//volatile uint8_t u8SetCosDirPositive    = 0;

volatile uint32_t u32Port1_IOCR0_Save   = 0;  // PORT 1 restore value for switching pattern
volatile uint32_t u32Port2_IOCR0_Save   = 0;  // PORT 2 restore value for switching pattern

uint32_t u32RevolutionStepsLimit;   // Steps per Revolution Limit -> direction change on demo mode
uint32_t u32RevolutionStepCounter;  // STep Counter per Revolution

volatile uint16_t i,jj;               // temp. indexer
volatile uint16_t temp1,temp2;        // temp for calc observation

volatile uint32_t u32TimeBasebySWTimer = 50; //Step Generation SW-Timer Threshold in [ms]

volatile uint8_t u8StepGenerationMethod = 2; /* 1:Step generation via internal timer
                                                2: Step generation via external input
                                             */
// internal oscilloscope
volatile uint8_t  u8OsziState              = 0;
volatile uint32_t u32MeasuretimerThreshold = 2; //1:250us
volatile uint32_t u16NumOfSamples          = 1200;
volatile uint16_t u16ScopeDataReadindex    = 0;
volatile uint16_t u16MeasureCounter;
volatile uint32_t u32Measuretimer_Tick;
volatile uint32_t u32Timer_1ms_Tick;
volatile uint32_t u32Timer_250us_Tick;


// internal UART COM to HOST
volatile uint32_t txcounter          = 50000;
volatile uint8_t u8ConnectionState   = 0;
volatile uint32_t u32UartRxTimeOut   = 0;
volatile uint8_t u8UartTxControl     = 0;
volatile uint32_t u32NothingReceived = 0;

volatile uint16_t dst,src;

/* read ChipID */
volatile uint32_t* CSIDptr;
volatile uint32_t ID_Value;
volatile uint32_t ID_Value_Adr = 0x10000FE8;

uint8_t eeprom_envolved = 0;

//--------------------------------------------------------------
// function prototypes
//--------------------------------------------------------------
void ISR_StepDetected(void);
void StepDetected_Fkt(void);

void SysTick_Handler(void);
void TxFinished(void);
void RxFinished(void);

void DoTransmission(void);
extern uint32_t XMC1000_CalcTemperature(void);

void ISR_UART_RxFinished(void);

//--------------------------------------------------------------
// Application entry point *************************************
//--------------------------------------------------------------
int main(void)
{//begin main

  DAVE_Init();           /* Initialization of DAVE APPs  */

  uint8_t data_from_eeprom[32] = {};

  E_EEPROM_XMC1_OPERATION_STATUS_t ret = E_EEPROM_XMC1_Read(UCB, 0u, data_from_eeprom, 32u);
  // has data in UCB eeprom
  if(ret == E_EEPROM_XMC1_OPERATION_STATUS_SUCCESS)
  {
	  eeprom_envolved = data_from_eeprom[27];

	  if(eeprom_envolved != 0u)
	  {
		  if(data_from_eeprom[17] != u8SteppingMode)
		  {
			  u8SteppingMode = data_from_eeprom[17];
		  }
		  if( (data_from_eeprom[18] != (uint8_t)(u32FreqPWMOutSin_Hz >> 24)) || (data_from_eeprom[19] != (uint8_t)(u32FreqPWMOutSin_Hz >> 16)) || \
				  (data_from_eeprom[20] != (uint8_t)(u32FreqPWMOutSin_Hz >> 8)) || (data_from_eeprom[21] != (uint8_t)(u32FreqPWMOutSin_Hz)) )
		  {
			  u32FreqPWMOutSin_Hz = ((uint32_t)data_from_eeprom[18] << 24) | ((uint32_t)data_from_eeprom[19] << 16) | ((uint32_t)data_from_eeprom[20] << 8) | ((uint32_t)data_from_eeprom[21]);
			  u32FreqPWMOutCos_Hz = u32FreqPWMOutSin_Hz;
		  }
		  if( (data_from_eeprom[22] != (uint8_t)(u32PWM_DutyCycleNormFactor >> 24)) || (data_from_eeprom[23] != (uint8_t)(u32PWM_DutyCycleNormFactor >> 16)) || \
				  (data_from_eeprom[24] != (uint8_t)(u32PWM_DutyCycleNormFactor >> 8)) || (data_from_eeprom[25] != (uint8_t)(u32PWM_DutyCycleNormFactor)) )
		  {
			  u32PWM_DutyCycleNormFactor = ((uint32_t)data_from_eeprom[22] << 24) | ((uint32_t)data_from_eeprom[23] << 16) | ((uint32_t)data_from_eeprom[24] << 8) | ((uint32_t)data_from_eeprom[25]);
			  f32Trimfaktor = u32PWM_DutyCycleNormFactor; //sets current limit also for microstep mode
		  }
		  if(data_from_eeprom[26] != (uint8_t)NumOfMicroSteps)
		  {
			  NumOfMicroSteps = data_from_eeprom[26];
		  }
	  }
  }

  ProbeScope_Init(1000);

// enbale StepInterrupt
  BCCU0->GLOBCON = BCCU0->GLOBCON | 0x00000040;
  BCCU0->CHOCON = BCCU0->CHOCON | 0x00010000;

  //flush USIC FIFOs
  XMC_USIC_CH_RXFIFO_Flush(XMC_UART0_CH1);
  XMC_USIC_CH_TXFIFO_Flush(XMC_UART0_CH1);

  //start DAC
  PWM_CCU4_Start(&PWMSinGen); //start PWM Generator for Sin Value generation
  PWM_CCU4_Start(&PWMCosGen); //start PWM Generator for Cos Value generation

  //start background conversion
  ADC_MEASUREMENT_ADV_StartADC(&ADC_MEASUREMENT_ADV_0);

  //Start Gate Control PWM Generators
  PWM_CCU8_Start(&PWMOutSin); // start GateControl PWM Sin-Phase
  PWM_CCU8_Start(&PWMOutCos); // start GateControl PWM Cos-Phase

  //DIGITAL_IO_SetOutputLow(&DisablePwrBridges); // disable Powerstage nur Testboard
  DIGITAL_IO_SetOutputHigh(&DisablePwrBridges); // disable Powerstage

  //start SYSTisk
   //PPB->SYST_RVR = 0x00FFFFFF;  // Systimer set to max
   PPB->SYST_RVR = 8000;          //8000 x 1/32MHz = 250us
   PPB->SYST_CSR = 7;             //Systimer starten

   /* Enable DTS */
    XMC_SCU_StartTempMeasurement();

    CSIDptr = (uint32_t*)ID_Value_Adr;
    ID_Value = *CSIDptr;
    /*if ID Value is 0x00020000 chip is A-Step TSE ROM-Function to read die temp does not work -> use workaround */

    src = 0;
    dst = 0;


  /*********************************************************************************/
  /* endless loop */
  /*********************************************************************************/

  /*main endless loop*/
  while(1U)
  {//begin endless

	  u32MainCounter++;

	  if(u8ControlMode == 1) // controlmode = Hardware
	  {
		  bDirection = (uint8_t)DIGITAL_IO_GetInput(&Input_DIR);
		  bEnable = (uint8_t)DIGITAL_IO_GetInput(&Input_DIS);

		  if( !bEnable)
		   {
			DIGITAL_IO_SetOutputHigh(&DisablePwrBridges); // Power Bridges OFF
		   }
	  }


	  if(u32MainCounter >2000)
	  {
		  u32MainCounter = 0;
		  /* Calculate temperature of the chip in Kelvin */

		  /* Enable DTS */
		  XMC_SCU_StartTempMeasurement();

		  // change GateTime Current Compare Blanking online
    	  u32FreqGateTime_Sin = 1000000 /  u32NoiseGatingTime_Sin_us;
    	  u32FreqGateTime_Cos = 1000000 /  u32NoiseGatingTime_Cos_us;
    	  PWM_CCU4_SetFreq(&NoiseBlankSin,u32FreqGateTime_Sin);
    	  PWM_CCU4_SetFreq(&NoiseBlankCos,u32FreqGateTime_Cos);

    	  // change PWM Frequency for PWM sin cos Generation online
    	  PWM_CCU4_SetFreq(&PWMSinGen,u32FreqSinGen_Hz);
    	  PWM_CCU4_SetFreq(&PWMCosGen,u32FreqCosGen_Hz);

    	  // change PWMFrequency and DutyCyle PWM Out sin cos online
    	  PWM_CCU8_SetFreqAsymmetric(&PWMOutSin,u32FreqPWMOutSin_Hz);
    	  PWM_CCU8_SetFreqAsymmetric(&PWMOutCos,u32FreqPWMOutCos_Hz);

    	  //The condition [shift + duty < 100%] should be met when APP operating in edge aligned mode
    	  PWM_CCU8_SetDutyCycleAsymmetric(&PWMOutSin,0,u32DCPWMOutSin);
    	  PWM_CCU8_SetDutyCycleAsymmetric(&PWMOutCos,0,u32DCPWMOutCos);

    	  u16ADCResult_Poti_akt_mV = (uint16_t)((uint32_t)u16ADCResult_Poti_akt * 5000U /4096U);
    	  u16ADCResult_Udc_akt_V   = (uint16_t)((uint32_t)u16ADCResult_Udc_akt * 50U /4096U);

    	  u16ADCResult_Isin_akt_mA = (uint16_t)((uint32_t)u16ADCResult_Isin_akt * 3000U /4096U);
    	  u16ADCResult_Icos_akt_mA = (uint16_t)((uint32_t)u16ADCResult_Icos_akt * 3000U /4096U);

    	  if(XMC_SCU_IsTempMeasurementDone())
    	  {
    	      // u32DieTemp_K = XMC_SCU_CalcTemperature(); but ROM Routine does not work see errata
    		  u32DieTemp_K =XMC1000_CalcTemperature();
    	     // Convert temperature from Kelvon to Celcius
    		  u16DieTemp_C = (uint16_t)(u32DieTemp_K - 273U);
    	  }
	  }


	  /*********************************************************************************/
	  /* Statemachine */
	  /*********************************************************************************/
	  switch(u8STMState)
	  {//begin STM
	    case 0: //Idle alles aus
	    {
	      u8STMState = 1;
	      break;
	    }

	    case 1://INIT all
	    {

	      u32RevolutionStepCounter = 0;
	      u32RevolutionStepsLimit = (uint32_t)u16NumOfStepsMotor * (uint32_t)u16Revolution;

	      jj=0;
	      //init microstep table in 4 sections from 0...1/2pi...pi...3/4pi...pi

	      fDeltaAngle = pi_2 / (float)(NumOfMicroSteps/4);  // calc incremental angle
	     	//0...pi/2
	        for(i=0; i<NumOfMicroSteps/4; i++)
	        {
	          //temp1 = (uint16_t)(sin(fAngle) * (float)((float)Imaxmot_mA * (float)ShuntResistor_mOhm) / ((float)ADCResolution12bit_uV * (float)InvCurrentAmplGain_k));
	          //temp2 = (uint16_t)(cos(fAngle) * (float)((float)Imaxmot_mA * (float)ShuntResistor_mOhm) / ((float)ADCResolution12bit_uV * (float)InvCurrentAmplGain_k));
		      temp1 = (uint16_t)(sin(fAngle) * f32Trimfaktor);
		      temp2 = (uint16_t)(cos(fAngle) * f32Trimfaktor);
	          atMicroStepSinCosTable[jj].u16Sin =temp1;
	          atMicroStepSinCosTable[jj].u16Cos =temp2;
	    	  //atMicroStepSinCosTable[i].u16SwitchCtrl = 0;//A+C sin-pos cos-pos
	    	  atMicroStepSinCosTable[jj].u16DirCtrl_Sin = 0x1U;
	    	  atMicroStepSinCosTable[jj].u16DirCtrl_Cos = 0x1U;
	    	  fAngle = fAngle + fDeltaAngle;
	    	  jj++;
	        }
	    	//pi/2...pi
	        for(i=0; i<NumOfMicroSteps/4; i++)
	        {
	          //temp1 = (uint16_t)(sin(fAngle) * (float)((float)Imaxmot_mA * (float)ShuntResistor_mOhm) / ((float)ADCResolution12bit_uV * (float)InvCurrentAmplGain_k));
	          //temp2 = (uint16_t)(-1.0 * (cos(fAngle) * (float)((float)Imaxmot_mA * (float)ShuntResistor_mOhm) / ((float)ADCResolution12bit_uV * (float)InvCurrentAmplGain_k)));
		      temp1 = (uint16_t)(sin(fAngle) * f32Trimfaktor);
		      temp2 = (uint16_t)(cos(fAngle) * -f32Trimfaktor);
	          atMicroStepSinCosTable[jj].u16Sin =temp1;
	          atMicroStepSinCosTable[jj].u16Cos =temp2;
	    	  //atMicroStepSinCosTable[i+StartSector_2].u16SwitchCtrl = 0;//A+D sin-pos cos-neg
	    	  atMicroStepSinCosTable[jj].u16DirCtrl_Sin = 0x1U;
	    	  atMicroStepSinCosTable[jj].u16DirCtrl_Cos = 0x10000U;
	    	  fAngle = fAngle + fDeltaAngle;
	    	  jj++;
	        }
	    	//pi...3/2pi
	        for(i=0; i<NumOfMicroSteps/4; i++)
	        {
	          //temp1 = (uint16_t)(-1.0 * (sin(fAngle) * (float)((float)Imaxmot_mA * (float)ShuntResistor_Ohm) / ((float)ADCResolution12bit_uV * (float)InvCurrentAmplGain_k)));  // sin is inverted
	          //temp2 = (uint16_t)(-1.0 * (cos(fAngle) * (float)((float)Imaxmot_mA * (float)ShuntResistor_Ohm) / ((float)ADCResolution12bit_uV * (float)InvCurrentAmplGain_k)));  // cos is inverted
		      temp1 = (uint16_t)(sin(fAngle) * -f32Trimfaktor);
		      temp2 = (uint16_t)(cos(fAngle) * - f32Trimfaktor);
	          atMicroStepSinCosTable[jj].u16Sin =temp1;
	          atMicroStepSinCosTable[jj].u16Cos =temp2;
	    	  //atMicroStepSinCosTable[i+StartSector_3].u16SwitchCtrl = 0;//B+D sin-neg cos-neg;
	    	  atMicroStepSinCosTable[jj].u16DirCtrl_Sin = 0x10000U;
	    	  atMicroStepSinCosTable[jj].u16DirCtrl_Cos = 0x10000U;
	    	  fAngle = fAngle + fDeltaAngle;
	    	  jj++;
	        }
	    	//3/2pi...2pi
	        for(i=0; i<NumOfMicroSteps/4; i++)
	        {
	          //temp1 = (uint16_t) (-1.0 * (sin(fAngle) * (float)((uint32_t)Imaxmot_mA * (float)ShuntResistor_Ohm) / ((float)ADCResolution12bit_uV * (float)InvCurrentAmplGain_k)));  // sin is inverted
	          //temp2 = (uint16_t)(cos(fAngle) * (float)((float)Imaxmot_mA * (float)ShuntResistor_Ohm) / ((float)ADCResolution12bit_uV * (float)InvCurrentAmplGain_k));
		      temp1 = (uint16_t)(sin(fAngle) * -f32Trimfaktor);
		      temp2 = (uint16_t)(cos(fAngle) * f32Trimfaktor);
	          atMicroStepSinCosTable[jj].u16Sin =temp1;
	          atMicroStepSinCosTable[jj].u16Cos =temp2;
	    	  //atMicroStepSinCosTable[i+StartSector_4].u16SwitchCtrl = 0;//B+C sin-neg cos-pos;
	    	  atMicroStepSinCosTable[jj].u16DirCtrl_Sin = 0x10000U;
	    	  atMicroStepSinCosTable[jj].u16DirCtrl_Cos = 0x1U;
	    	  fAngle = fAngle + fDeltaAngle;
	    	  jj++;
	        }

	      u16LatchIndex = 0;
	      s16Index = 0;

	      u8STMState = 2;
	    break;
	    }

	    case 2://Calibration Meassure Current Offset start
	    {
	      u8STMState = 3;// current offset detection done switch to next step
	      // not implemented so far
	      break;
	    }
	    case 3://Meassure Current Offset wait to finish all measurements
	    {
	      u8STMState = 4;// current offset detection done switch to next step
  	      break;
	    }

	    case 4://Normal Operation
	    {
	    	u16ADCResult_Udc_akt  = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_CH3_Udc);
	    	u16ADCResult_Poti_akt = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_CH4_Poti);

	    	u32PWM_DutyCycle_FullHalf_Sin = u16ADCResult_Poti_akt * u32PWM_DutyCycleNormFactor /4096;
	    	u32PWM_DutyCycle_FullHalf_Cos = u16ADCResult_Poti_akt * u32PWM_DutyCycleNormFactor /4096;

	    	//u16ADCResult_Isin_akt = au16ADC_Results[u16LatchIndex].u16_ISin;
	    	//u16ADCResult_Icos_akt = au16ADC_Results[u16LatchIndex].u16_ICos;

	    	/****** Receive Data From Host PC ********/
	    	Uart_RxState = UART_Receive(&Com2Host, (uint8_t*)(RxData_FromPC.au8ReceiveDataAll), 32);

	    	if(Uart_RxState == UART_STATUS_BUSY)
            {
              u32UartRxTimeOut++;
              if(u32UartRxTimeOut > 70000)
              {
            	  u8UartTxControl = 0;
            	  u32UartRxTimeOut = 0;
            	  u8ConnectionState = 0;
            	  DIGITAL_IO_SetOutputHigh(&DiagLED);
              }
            }
        
	    	if(Uart_RxState == UART_STATUS_SUCCESS)
	    	{// begin data successfully received

	    		 DIGITAL_IO_ToggleOutput(&DiagLED);

	    		 u32RxFinishedCounterMain++;

                 u8ConnectionState = 100;
                 u8UartTxControl = 1;

                 for(i=0; i<28;i++)
	    	     {
	    	    	 RxData_FromPC.au8ReceiveData[i] = RxData_FromPC.au8ReceiveDataAll[i];
	    	     }

	    	     RxData_FromPC.u8NumOfRxBlocks = RxData_FromPC.au8ReceiveDataAll[28];
	    	     RxData_FromPC.u8RemainingRxData= RxData_FromPC.au8ReceiveDataAll[29];
	    	     RxData_FromPC.u8DataMode = RxData_FromPC.au8ReceiveDataAll[30];
	    	     RxData_FromPC.u8RxCounter = RxData_FromPC.au8ReceiveDataAll[31];
	    	     u32UartRxTimeOut = 0;


	    	  switch(RxData_FromPC.u8DataMode)
	    	  {//begin switch
	    	  case 50:
	    	  {

	    			 TxData_ToPC.u8DataMode = 50;  //diagnose daten
	    		     //main counter
	    		     TxData_ToPC.au8SendData[0] =    (uint8_t)u32MainCounter;
	    		     TxData_ToPC.au8SendData[1] =	 (uint8_t)u32MainCounter >> 8;
	    	   	     TxData_ToPC.au8SendData[2] =    (uint8_t)u32MainCounter >> 16;
	    	   	     TxData_ToPC.au8SendData[3] =	 (uint8_t)u32MainCounter >> 24;
	    	         //DieTemp
	    	     	 TxData_ToPC.au8SendData[4] =	 (uint8_t)u16DieTemp_C;
	    	    	 TxData_ToPC.au8SendData[5] =	 (uint8_t)(u16DieTemp_C >> 8);
	    		     // UDC-Link
	    	   	     TxData_ToPC.au8SendData[6] =	 (uint8_t)u16ADCResult_Udc_akt_V;
	    	  	     TxData_ToPC.au8SendData[7] =	 (uint8_t)(u16ADCResult_Udc_akt_V >> 8);
	    		     // IMot_avr
	    	   	     TxData_ToPC.au8SendData[8] =	 0;
	    	  	     TxData_ToPC.au8SendData[9] =	 1;
	    		     // ImotSet
	    	   	     TxData_ToPC.au8SendData[10] =	 (uint8_t)u16ADCResult_Poti_akt_mV;
	    	  	     TxData_ToPC.au8SendData[11] =	 (uint8_t)(u16ADCResult_Poti_akt_mV >> 8);

	    		     // STMState
	    	   	     TxData_ToPC.au8SendData[12] =	 (uint8_t)u8STMState;
	    	  	    //SteppingMode
	    	   	     TxData_ToPC.au8SendData[13] =	 (uint8_t)u8SteppingMode;
	    		     // Direction
	    	   	     TxData_ToPC.au8SendData[14] =	 (uint8_t)bDirection;
	    	  	     //Enable
	    	   	     TxData_ToPC.au8SendData[15] =	 (uint8_t)bEnable;

	    	   	     //OsziState
	    	   	 	 TxData_ToPC.au8SendData[16] =	 (uint8_t)u8OsziState;

	    			  DoTransmission();
	    			  src = 0;
	    			  dst = 0;
	    		  break;
	    	  }
	    	  case 55:
	    	  {

		    	if(u8OsziState == 2)
		    	{// send Oszi data
		    		TxData_ToPC.u8DataMode = 55;  //diagnose daten
		    		if(TxData_ToPC.u8TxCounter <=  TxData_ToPC.u8NumOfTxBlocks)
		    		{

		    			TxData_ToPC.u8TxCounter++;
		    			for(i=0; i<14;i=i+2)
		    			{
		    				TxData_ToPC.au16SendData[i]   = au16ADC_Results[u16ScopeDataReadindex].u16_K1;
		    				TxData_ToPC.au16SendData[i+1] = au16ADC_Results[u16ScopeDataReadindex].u16_K2;
		    				u16ScopeDataReadindex++;
		    			}
		    			DoTransmission();
		    		}
		    		else
		    		{
		    			u8OsziState = 0;
		    			u16ScopeDataReadindex = 0;
		    			TxData_ToPC.u8DataMode = 50;  //diagnose daten

		    		}
		    	}
	    	  }break;


	    	  case 10:
	    	    {//MotorData

	    			  MotorData.u16MotorType    = (uint16_t)((uint16_t)RxData_FromPC.au8ReceiveData[1] << 8 | (uint16_t)RxData_FromPC.au8ReceiveData[0]);
	    			  MotorData.u16StepsPerRef  = (uint16_t)((uint16_t)RxData_FromPC.au8ReceiveData[3] << 8 | (uint16_t)RxData_FromPC.au8ReceiveData[2]);
	    			  MotorData.u16MotorSpeed   = (uint16_t)((uint16_t)RxData_FromPC.au8ReceiveData[5] << 8 | (uint16_t)RxData_FromPC.au8ReceiveData[4]);
	    			  MotorData.u16MotorCurrent = (uint16_t)((uint16_t)RxData_FromPC.au8ReceiveData[7] << 8 | (uint16_t)RxData_FromPC.au8ReceiveData[6]);
	    		  break;
	    	    }

	    	  case 20:
	    	    {//BoardData

	    	    	  BoardData.u16_f_IrefSin    = (uint16_t)((uint16_t)RxData_FromPC.au8ReceiveData[1] << 8 | (uint16_t)RxData_FromPC.au8ReceiveData[0]);
	    	    	  BoardData.u16_f_IrefCos    = (uint16_t)((uint16_t)RxData_FromPC.au8ReceiveData[3] << 8 | (uint16_t)RxData_FromPC.au8ReceiveData[2]);
	    	    	  BoardData.u16MinOnTimeSin  = (uint16_t)((uint16_t)RxData_FromPC.au8ReceiveData[5] << 8 | (uint16_t)RxData_FromPC.au8ReceiveData[4]);
	    	    	  BoardData.u16MinOnTimeCos  = (uint16_t)((uint16_t)RxData_FromPC.au8ReceiveData[7] << 8 | (uint16_t)RxData_FromPC.au8ReceiveData[6]);
	    	    	  BoardData.u16FixOffTimeSin = (uint16_t)((uint16_t)RxData_FromPC.au8ReceiveData[9] << 8 | (uint16_t)RxData_FromPC.au8ReceiveData[8]);
	    	    	  BoardData.u16FixOffTimeCos = (uint16_t)((uint16_t)RxData_FromPC.au8ReceiveData[11] << 8 | (uint16_t)RxData_FromPC.au8ReceiveData[10]);
	    	    	  BoardData.u16_f_ChopSin    = (uint16_t)((uint16_t)RxData_FromPC.au8ReceiveData[13] << 8 | (uint16_t)RxData_FromPC.au8ReceiveData[12]);
	    	    	  BoardData.u16_f_ChopCos    = (uint16_t)((uint16_t)RxData_FromPC.au8ReceiveData[15] << 8 | (uint16_t)RxData_FromPC.au8ReceiveData[14]);

	    	    	  u32FreqSinGen_Hz = BoardData.u16_f_IrefSin * 1000;
	    	    	  u32FreqCosGen_Hz = BoardData.u16_f_IrefCos * 1000;

	    	    	  u32NoiseGatingTime_Sin_us = BoardData.u16MinOnTimeSin;
	    	    	  u32NoiseGatingTime_Cos_us = BoardData.u16MinOnTimeCos;

	    	    	  u32DCPWMOutSin = BoardData.u16FixOffTimeSin * 100; //dutycycle
					  u32DCPWMOutCos = BoardData.u16FixOffTimeCos * 100; //dutycycle

	    	    	  u32FreqPWMOutSin_Hz = BoardData.u16_f_ChopSin * 1000;
					  u32FreqPWMOutCos_Hz = BoardData.u16_f_ChopCos * 1000;


	    	      break;
	    	    }

	    	  case 30:
	    	    {//StepData

	    	    	  StepData.u16WaveFormNo         = (uint16_t)((uint16_t)RxData_FromPC.au8ReceiveData[1] << 8 | (uint16_t)RxData_FromPC.au8ReceiveData[0]);
	    	    	  StepData.u16NumOfMicroSteps    = (uint16_t)((uint16_t)RxData_FromPC.au8ReceiveData[3] << 8 | (uint16_t)RxData_FromPC.au8ReceiveData[2]);
	    	    	  StepData.u16OverCurrentFactor  = (uint16_t)((uint16_t)RxData_FromPC.au8ReceiveData[5] << 8 | (uint16_t)RxData_FromPC.au8ReceiveData[4]);
	    	    	  StepData.u8Direction           = (uint8_t)RxData_FromPC.au8ReceiveData[6];



	    	    	  if(StepData.u8Direction == 0)
	    	    	  {
	    	    		  bDirection = 0;
	    	    	  }
	    	    	  else
	    	    	  {
	    	    		  bDirection = 1;
	    	    	  }

	    	    	  if((StepData.u16WaveFormNo >=1) && (StepData.u16WaveFormNo <=6))
	    	    	  {
	    	    	    u8SteppingMode = StepData.u16WaveFormNo;
	    	    	  }

	    	    	  if((StepData.u16NumOfMicroSteps >= 8)&&(StepData.u16NumOfMicroSteps <= 64))
	    	    	  {
	    	    		  NumOfMicroSteps = StepData.u16NumOfMicroSteps;
	    	    	  }


	    	    	  u8STMState = 0; //reinit all


       		       break;
	    	    }

	    	  case 40:
	    	    {//Control Data
	    	    	ControlData.u8EnableMotor        = (uint8_t)RxData_FromPC.au8ReceiveData[0];
	    	    	ControlData.u8free1              = (uint8_t)RxData_FromPC.au8ReceiveData[1];
                    bEnable = ControlData.u8EnableMotor;

	    		  break;
	    	    }

	    	  case 41:
	    	    {//WaveForm Change Data

	    	    	//send first block
	    	    	if (RxData_FromPC.u8RxCounter < RxData_FromPC.u8NumOfRxBlocks)
	    	    	{
	    	    	    for (i = 0; i < 7; i++)
	    	    	    {
	    	    	        //sin_1

							atMicroStepSinCosTable[dst].u16SinMod   = (uint16_t)((uint16_t)RxData_FromPC.au8ReceiveData[src+1] << 8 | (uint16_t)RxData_FromPC.au8ReceiveData[src]);
	    	    	    	//cos_1
	    	    	    	atMicroStepSinCosTable[dst].u16CosMod  = (uint16_t)((uint16_t)RxData_FromPC.au8ReceiveData[src+3] << 8 | (uint16_t)RxData_FromPC.au8ReceiveData[src+2]);

	    	    	    	src = src + 4;
	    	    	        dst = dst + 1;
	    	    	    }
	    	    	    src = 0;
	    	    	}
	    	    	else if (RxData_FromPC.u8RemainingRxData > 0)
	    	    	{
	    	    	    for (i = 0; i < RxData_FromPC.u8RemainingRxData; i++)
	    	    	    {
	    	    	        //sin_1
	    	    	    	atMicroStepSinCosTable[dst].u16SinMod   = (uint16_t)((uint16_t)RxData_FromPC.au8ReceiveData[src+1] << 8 | (uint16_t)RxData_FromPC.au8ReceiveData[src]);
	    	    	        //cos_1
	    	    	    	atMicroStepSinCosTable[dst].u16CosMod  = (uint16_t)((uint16_t)RxData_FromPC.au8ReceiveData[src+3] << 8 | (uint16_t)RxData_FromPC.au8ReceiveData[src+2]);
	    	    	    }
	    	    	    src = 0;
	    	    	    dst = 0;
	    	    	}
	    		  break;
	    	     }

	    	  case 42:
	    	    {//Speed
	    	    	u32TimeBasebySWTimer = (uint32_t)((uint16_t)RxData_FromPC.au8ReceiveData[1] << 8 | (uint16_t)RxData_FromPC.au8ReceiveData[0]);
	    		  break;
	    	    }

	    	  case 43:
	    	    {//Scope starten

	    	    	if(u8OsziState == 0)
	    	    	{// scope idle
	    	    	  ScopeData.u8Ch1SignalIdx    = (uint8_t)RxData_FromPC.au8ReceiveData[0];
	    	    	  ScopeData.u8Ch2SignalIdx    = (uint8_t)RxData_FromPC.au8ReceiveData[1];
	    	    	  ScopeData.u8NumOfSamplesIdx = (uint8_t)RxData_FromPC.au8ReceiveData[2];
	    	    	  ScopeData.u8SampleTimeIdx   = (uint8_t)RxData_FromPC.au8ReceiveData[3];
	    	    	  ScopeData.u8Control         = (uint8_t)RxData_FromPC.au8ReceiveData[4];

	    	    	  switch(ScopeData.u8NumOfSamplesIdx)
	    	    	  {
	    	    	  case 0: u16NumOfSamples = 175; //250us
	    	    	          break;
	    	    	  case 1: u16NumOfSamples = 350; //500us
	    	    	          break;
	    	    	  case 2: u16NumOfSamples = 700; //750us
	    	    	          break;
	    	    	  case 3: u16NumOfSamples = 1050; //1000us
	    	    	          break;
	    	    	  case 4: u16NumOfSamples = 1400; //2000us
	    	    	          break;
	    	    	  default: u16NumOfSamples = 1400;//5000us
	    	    	          break;
	    	    	  }

	    	    	  switch(ScopeData.u8SampleTimeIdx)
	    	    	  {
	    	    	  case 0: u32MeasuretimerThreshold = 1; //250us
	    	    	          break;
	    	    	  case 1: u32MeasuretimerThreshold = 2; //500us
	    	    	          break;
	    	    	  case 2: u32MeasuretimerThreshold = 3; //750us
	    	    	          break;
	    	    	  case 3: u32MeasuretimerThreshold = 4; //1000us
	    	    	          break;
	    	    	  case 4: u32MeasuretimerThreshold = 8; //2000us
	    	    	          break;
	    	    	  case 5: u32MeasuretimerThreshold = 20;//5000us
	    	    	          break;
	    	    	  default: u32MeasuretimerThreshold = 4;//5000us
	    	    	          break;
	    	    	  }

	    	    	  if(ScopeData.u8Control == 1)
	    	    	  {
	    	    		u32Measuretimer_Tick = 0;
       	    		    u16MeasureCounter = 0;
       	    		    u16LatchIndex = 0;
       	    		    u8OsziState = 1;
	    	    	  }
	    	    	}// end if idle


	    		  break;
	    	    }
	    	  case 165: // = 0xA5
	    	  {
		    	StepData.u16WaveFormNo = RxData_FromPC.au8ReceiveDataAll[17];
	    		u32FreqPWMOutSin_Hz =(uint32_t)((uint32_t)RxData_FromPC.au8ReceiveDataAll[18] << 24 | \
												  (uint32_t)RxData_FromPC.au8ReceiveDataAll[19] << 16 | \
												  (uint32_t)RxData_FromPC.au8ReceiveDataAll[20] << 8 | \
												  (uint32_t)RxData_FromPC.au8ReceiveDataAll[21] );
	    		u32FreqPWMOutCos_Hz = u32FreqPWMOutSin_Hz;
				uint32_t received_PWM_DutyCycleNormFactor = (uint32_t)((uint32_t)RxData_FromPC.au8ReceiveDataAll[22] << 24 | \
																		  (uint32_t)RxData_FromPC.au8ReceiveDataAll[23] << 16 | \
																		  (uint32_t)RxData_FromPC.au8ReceiveDataAll[24] << 8 | \
																		  (uint32_t)RxData_FromPC.au8ReceiveDataAll[25] );
				StepData.u16NumOfMicroSteps = RxData_FromPC.au8ReceiveDataAll[26];


	    	    eeprom_envolved = (RxData_FromPC.au8ReceiveDataAll[27] & 1u);

				if((StepData.u16WaveFormNo == 1) || (StepData.u16WaveFormNo == 2) || (StepData.u16WaveFormNo == 4))
				{
				  u8SteppingMode = StepData.u16WaveFormNo;
				}

				if( (received_PWM_DutyCycleNormFactor != 0u) && (received_PWM_DutyCycleNormFactor < 10000u) )
				{
					u32PWM_DutyCycleNormFactor = received_PWM_DutyCycleNormFactor;
					f32Trimfaktor = u32PWM_DutyCycleNormFactor; //sets current  limit also for microstep mode
				}

				if((StepData.u16NumOfMicroSteps >= 8)&&(StepData.u16NumOfMicroSteps <= 64))
				{
				  NumOfMicroSteps = StepData.u16NumOfMicroSteps;
				}

				if(eeprom_envolved)
				{
					uint8_t data_to_eeprom[32] = {};

					data_to_eeprom[17] =	 (uint8_t)u8SteppingMode;
					data_to_eeprom[18] =	 (uint8_t)(u32FreqPWMOutSin_Hz	>> 24);
					data_to_eeprom[19] =	 (uint8_t)(u32FreqPWMOutSin_Hz 	>> 16);
					data_to_eeprom[20] =	 (uint8_t)(u32FreqPWMOutSin_Hz 	>> 8);
					data_to_eeprom[21] =	 (uint8_t)u32FreqPWMOutSin_Hz;
					data_to_eeprom[22] =	 (uint8_t)(u32PWM_DutyCycleNormFactor	>> 24);
					data_to_eeprom[23] =	 (uint8_t)(u32PWM_DutyCycleNormFactor 	>> 16);
					data_to_eeprom[24] =	 (uint8_t)(u32PWM_DutyCycleNormFactor 	>> 8);
					data_to_eeprom[25] =	 (uint8_t)u32PWM_DutyCycleNormFactor;
					data_to_eeprom[26] =	 (uint8_t)NumOfMicroSteps;

					data_to_eeprom[27] = eeprom_envolved;

					data_to_eeprom[30] = RxData_FromPC.u8DataMode;

					(void)E_EEPROM_XMC1_Write(UCB, data_to_eeprom);
				}
				u8STMState = 0u;

	    		break;
	    	  }

	    	  case 166: // = 0xA6
	    	  {
				TxData_ToPC.au8SendData[17] =	 (uint8_t)u8SteppingMode;
				TxData_ToPC.au8SendData[18] =	 (uint8_t)(u32FreqPWMOutSin_Hz	>> 24);
				TxData_ToPC.au8SendData[19] =	 (uint8_t)(u32FreqPWMOutSin_Hz 	>> 16);
				TxData_ToPC.au8SendData[20] =	 (uint8_t)(u32FreqPWMOutSin_Hz 	>> 8);
				TxData_ToPC.au8SendData[21] =	 (uint8_t)u32FreqPWMOutSin_Hz;
				TxData_ToPC.au8SendData[22] =	 (uint8_t)(u32PWM_DutyCycleNormFactor	>> 24);
				TxData_ToPC.au8SendData[23] =	 (uint8_t)(u32PWM_DutyCycleNormFactor 	>> 16);
				TxData_ToPC.au8SendData[24] =	 (uint8_t)(u32PWM_DutyCycleNormFactor 	>> 8);
				TxData_ToPC.au8SendData[25] =	 (uint8_t)u32PWM_DutyCycleNormFactor;
				TxData_ToPC.au8SendData[26] =	 (uint8_t)NumOfMicroSteps;

				TxData_ToPC.au8SendData[27] = eeprom_envolved;

				TxData_ToPC.u8DataMode = RxData_FromPC.u8DataMode;

				DoTransmission();

	    		break;
	    	  }
	    	  default:
	    	    {
                  //fail
	    	      u32NothingReceived++;
	    		  break;
	    	    }
	    	  }//end switch
	    	}// end data successfully received



	    break;
	    }

	    case 5: //Fail
	    {
	  	  if(bEnable)
	  	  {
	  		u8STMState = 6;
	  	  }
	  	  else
	  	  {
            ;
	  	  }
	    }
	    case 6:
	    {
	     ; // wait
	       break;
	    }

	    default:
	    {
	      ; // wait
	      break;
	    }
	  }//end STM



  }//end endless
}//end main


void DoTransmission(void)
{

	//TxData_ToPC.u8TxCounter++;
      for(i=0; i<28;i++)
      {
 	   TxData_ToPC.au8SendDataAll[i] = TxData_ToPC.au8SendData[i];
      }

      TxData_ToPC.au8SendDataAll[28] = TxData_ToPC.u8NumOfTxBlocks;
      TxData_ToPC.au8SendDataAll[29] = TxData_ToPC.u8RemainingTxData;
      TxData_ToPC.au8SendDataAll[30] = TxData_ToPC.u8DataMode;
      TxData_ToPC.au8SendDataAll[31] = TxData_ToPC.u8TxCounter;

      if(u8UartTxControl == 1)
      {
        Uart_TxState = UART_Transmit(&Com2Host, (uint8_t*)&TxData_ToPC, 32);
      }
}



void SysTick_Handler(void)
{
  //this function is called by SW Timer every 1ms
  u32Timer_1ms_Tick++;
  u32Timer_250us_Tick++;


  //if(bEnable)
  if(1)
    {//enabled
	  if(u8OsziState == 1)
	  {
		  u16MeasureCounter++;
		  u32Measuretimer_Tick++;
		if((u32Measuretimer_Tick >= u32MeasuretimerThreshold))
	    {
	    	u32Measuretimer_Tick = 0;
            switch(ScopeData.u8Ch1SignalIdx)
            {
            //0: OFF
            case 0:  au16ADC_Results[u16LatchIndex].u16_K1 = 0; break;
            //1: I_Sin
            case 1:  au16ADC_Results[u16LatchIndex].u16_K1 = ADC_MEASUREMENT_ADV_GetResult(&ADC_MEASUREMENT_ADV_1_CH1_ISin); break;
            //2: I_Cos
            case 2:  au16ADC_Results[u16LatchIndex].u16_K1 = ADC_MEASUREMENT_ADV_GetResult(&ADC_MEASUREMENT_ADV_0_CH2_ICos); break;
            //3: I_RefSin
            case 3:  au16ADC_Results[u16LatchIndex].u16_K1 = ADC_MEASUREMENT_ADV_GetResult(&ADC_MEASUREMENT_ADV_0_CHRef_Sin); break;
            //4: I_RefCos
            case 4:  au16ADC_Results[u16LatchIndex].u16_K1 = ADC_MEASUREMENT_ADV_GetResult(&ADC_MEASUREMENT_ADV_1_CHRef_Cos); break;
            //5: U_DCLink
            case 5: au16ADC_Results[u16LatchIndex].u16_K1 = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_CH3_Udc); break;
            //6: U_Poti
            case 6:  au16ADC_Results[u16LatchIndex].u16_K1 = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_CH4_Poti); break;
            //7: StepSignal
            case 7:  au16ADC_Results[u16LatchIndex].u16_K1 = PPB->SYST_CVR;break; //Systimerwert latchen; break;
            //8: TimerTick
            case 8:  au16ADC_Results[u16LatchIndex].u16_K1 = u16MeasureCounter; break;
            default: break;
            }

            switch(ScopeData.u8Ch2SignalIdx)
            {
            //0: OFF
            case 0:  au16ADC_Results[u16LatchIndex].u16_K2 = 0; break;
            //1: I_Sin
            case 1:  au16ADC_Results[u16LatchIndex].u16_K2= ADC_MEASUREMENT_ADV_GetResult(&ADC_MEASUREMENT_ADV_1_CH1_ISin); break;
            //2: I_Cos
            case 2:  au16ADC_Results[u16LatchIndex].u16_K2 = ADC_MEASUREMENT_ADV_GetResult(&ADC_MEASUREMENT_ADV_0_CH2_ICos); break;
            //3: I_RefSin
            case 3:  au16ADC_Results[u16LatchIndex].u16_K2 = ADC_MEASUREMENT_ADV_GetResult(&ADC_MEASUREMENT_ADV_0_CHRef_Sin); break;
            //4: I_RefCos
            case 4:  au16ADC_Results[u16LatchIndex].u16_K2 = ADC_MEASUREMENT_ADV_GetResult(&ADC_MEASUREMENT_ADV_1_CHRef_Cos); break;
            //5: U_DCLink
            case 5: au16ADC_Results[u16LatchIndex].u16_K2 = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_CH3_Udc); break;
            //6: U_Poti
            case 6:  au16ADC_Results[u16LatchIndex].u16_K2 = ADC_MEASUREMENT_GetResult(&ADC_MEASUREMENT_CH4_Poti); break;
            //7: StepSignal
            case 7:  au16ADC_Results[u16LatchIndex].u16_K2 = PPB->SYST_CVR;break; //Systimerwert latchen; break;
            //8: TimerTick
            case 8:  au16ADC_Results[u16LatchIndex].u16_K2 = u16MeasureCounter; break;
            default: break;
            }

 	      u16LatchIndex++;
 	      if(u16LatchIndex >= u16NumOfSamples)
 	      {
 		    u16LatchIndex = 0;
 		    TxData_ToPC.u8NumOfTxBlocks = u16NumOfSamples / 7;
 		    TxData_ToPC.u8DataMode = 55;
 		    TxData_ToPC.u8TxCounter = 0;
 		    u16ScopeDataReadindex = 0;
 		    u8OsziState = 2;
 	      }
	    }
	  }
    }

  //build 1ms
  if(u32Timer_1ms_Tick >=4)
  {
	  //DIGITAL_IO_ToggleOutput(&DiagLED);
	  ADC_MEASUREMENT_StartConversion(&ADC_MEASUREMENT_0);
	  ProbeScope_Sampling();
	  u32Timer_1ms_Tick = 0;
  }

  //build xtimes 250us
  if(u32Timer_250us_Tick >=u32TimeBasebySWTimer)
  {
	  u32Timer_250us_Tick = 0;
	  if(u8StepGenerationMethod == 1 )
	  {
		  StepDetected_Fkt();
	  }
  }
}


//--------------------------------------------------------------
// Interrupt routine to handle stepwise motor movement
//--------------------------------------------------------------
void ISR_StepDetected(void)
{
	  if(u8StepGenerationMethod == 2)
	  {
		  StepDetected_Fkt();
	  }

}

void StepDetected_Fkt(void)
{

  u16ISRStepCounter++;

  if(u8ControlMode == 1) // controlmode = hW
  {
	  bDirection = (uint8_t)DIGITAL_IO_GetInput(&Input_DIR);
	  bEnable = (uint8_t)DIGITAL_IO_GetInput(&Input_DIS);
  }

  if(bEnable)
  {//enabled
    DIGITAL_IO_SetOutputHigh(&TestOutput);


    DIGITAL_IO_SetOutputLow(&DisablePwrBridges); // Power Bridges ON

    /*
	  au16ADC_Results[u16LatchIndex].u16_ISin = ADC_MEASUREMENT_ADV_GetResult(&ADC_MEASUREMENT_ADV_1_CH1_ISin);
	  au16ADC_Results[u16LatchIndex].u16_ICos = ADC_MEASUREMENT_ADV_GetResult(&ADC_MEASUREMENT_ADV_0_CH2_ICos);
	  //au16ADC_Results[u8LatchIndex].u32SampleTick = PPB->SYST_CVR; //Systimerwert latchen
	  u16LatchIndex = ((u16LatchIndex+1) & 0x03FF); // wrap around at 1024
     */

    // select from switching mode
	switch(u8SteppingMode)
    {
      case 1://Fullstep
      {
    	//setvalue for Sin and Cos current control
    	PWM_CCU4_SetDutyCycle(&PWMSinGen,u32PWM_DutyCycle_FullHalf_Sin);
    	PWM_CCU4_SetDutyCycle(&PWMCosGen,u32PWM_DutyCycle_FullHalf_Cos);

        // direction sin
    	//0x1U:     set P0.12 to high => L/R-Dir Control = 1 => pos Current in SIN coil
    	//0x10000U: set P0.12 to low  => L/R-Dir Control = 1 => pos Current in SIN coil
    	PORT0->OMR = ((uint32_t)FullStepSwitching[s16Index].u16DirCtrl_Sin << 12);

    	// direction cos
    	//0x1U:     set P0.8 to high => L/R-Dir Control = 1 => pos Current in COS coil
    	//0x10000U: set P0.8 to low  => L/R-Dir Control = 1 => pos Current in COS coil
    	PORT0->OMR = ((uint32_t)FullStepSwitching[s16Index].u16DirCtrl_Cos << 8);

    	//PWM sin
    	u32Port1_IOCR0_Save = PORT1->IOCR0;
    	u32Port1_IOCR0_Save &= (uint32_t)(0xFFFFFF00);
    	u32Port1_IOCR0_Save|= (uint32_t)FullStepSwitching[s16Index].u16PWMCtrlON_Sin;
    	PORT1->IOCR0 = u32Port1_IOCR0_Save;

    	//PWM cos
    	u32Port2_IOCR0_Save = PORT2->IOCR0;
    	u32Port2_IOCR0_Save &= (uint32_t)(0xFFFFFF00);
    	u32Port2_IOCR0_Save|= (uint32_t)FullStepSwitching[s16Index].u16PWMCtrlON_Cos;
    	PORT2->IOCR0 = u32Port2_IOCR0_Save;


        if(bDirection == 0)
        {
          s16Index--;
        }
        else
        {
          s16Index++;
        }


        if(s16Index == 4)
        {
          s16Index = 0;
        }

	    if(s16Index ==  -1)
        {
          s16Index = 3;
        }


	   // u32RevolutionStepCounter++;
	    if(u32RevolutionStepCounter > u32RevolutionStepsLimit)
	    {
		  u32RevolutionStepCounter = 0;
		  if(bDirection == 0)
		  {
	        bDirection = 1;
	        s16Index = 3;
		  }
	      else
		  {
	        bDirection = 0;
	        s16Index = 0;
		  }
	    }
        break;
      }

    case 2://Halfstep
      {
    	//setvalue for Sin and Cos current control
    	PWM_CCU4_SetDutyCycle(&PWMSinGen,u32PWM_DutyCycle_FullHalf_Sin);
      	PWM_CCU4_SetDutyCycle(&PWMCosGen,u32PWM_DutyCycle_FullHalf_Cos);

      	// direction sin
      	//0x1U:     set P0.12 to high => L/R-Dir Control = 1 => pos Current in SIN coil
      	//0x10000U: set P0.12 to low => L/R-Dir Control = 1 => pos Current in SIN coil
      	PORT0->OMR = ((uint32_t)HalfStepSwitching[s16Index].u16DirCtrl_Sin << 12);

      	// direction cos
      	//0x1U:     set P0.8 to high => L/R-Dir Control = 1 => pos Current in COS coil
      	//0x10000U: set P0.8 to low => L/R-Dir Control = 1 => pos Current in COS coil
      	PORT0->OMR = ((uint32_t)HalfStepSwitching[s16Index].u16DirCtrl_Cos << 8);

      	//PWM sin
      	u32Port1_IOCR0_Save = PORT1->IOCR0;
      	u32Port1_IOCR0_Save &= (uint32_t)(0xFFFFFF00);
      	u32Port1_IOCR0_Save|= (uint32_t)HalfStepSwitching[s16Index].u16PWMCtrlON_Sin;
      	PORT1->IOCR0 = u32Port1_IOCR0_Save;

      	//PWM cos
      	u32Port2_IOCR0_Save = PORT2->IOCR0;
      	u32Port2_IOCR0_Save &= (uint32_t)(0xFFFFFF00);
      	u32Port2_IOCR0_Save|= (uint32_t)HalfStepSwitching[s16Index].u16PWMCtrlON_Cos;
      	PORT2->IOCR0 = u32Port2_IOCR0_Save;

          if(bDirection == 0)
          {
            s16Index--;
          }
          else
          {
            s16Index++;
          }

    	  if(s16Index == 8)
          {
            s16Index = 0;
          }
          if(s16Index ==  -1)
          {
            s16Index = 7;
          }

    	  //u32RevolutionStepCounter++;
    	  if(u32RevolutionStepCounter > u32RevolutionStepsLimit)
    	  {
    		u32RevolutionStepCounter = 0;
    		if(bDirection == 0)
    		{
    	      bDirection = 1;
    	      s16Index = 7;
    		}
    	    else
    		{
    	      bDirection = 0;
    	      s16Index = 0;
    		}
    	  }
          break;
      }
    case 4://Microstep sin cos
     {
       //setvalue for Sin and Cos current control
       u32PWM_DutyCycle_Sin = (uint32_t)(((uint32_t)atMicroStepSinCosTable[s16Index].u16Sin * (uint32_t)u16ADCResult_Poti_akt) >> 12U);
  	   u32PWM_DutyCycle_Cos = (uint32_t)(((uint32_t)atMicroStepSinCosTable[s16Index].u16Cos * (uint32_t)u16ADCResult_Poti_akt) >> 12U);

       PWM_CCU4_SetDutyCycle(&PWMSinGen,u32PWM_DutyCycle_Sin);
       PWM_CCU4_SetDutyCycle(&PWMCosGen,u32PWM_DutyCycle_Cos);


       // direction sin
   	   //0x1U:     set P0.12 to high => L/R-Dir Control = 1 => pos Current in SIN coil
   	   //0x10000U: set P0.12 to low => L/R-Dir Control = 1 => pos Current in SIN coil
   	   PORT0->OMR = ((uint32_t)atMicroStepSinCosTable[s16Index].u16DirCtrl_Sin << 12);

   	   // direction cos
   	   //0x1U:     set P0.8 to high => L/R-Dir Control = 1 => pos Current in COS coil
   	   //0x10000U: set P0.8 to low => L/R-Dir Control = 1 => pos Current in COS coil
   	   PORT0->OMR = ((uint32_t)atMicroStepSinCosTable[s16Index].u16DirCtrl_Cos << 8);

       //PWM sin
       u32Port1_IOCR0_Save = PORT1->IOCR0;
       u32Port1_IOCR0_Save &= (uint32_t)(0xFFFFFF00);
       if(u32PWM_DutyCycle_Sin <= u32CompareThreshold)
       {
         u32Port1_IOCR0_Save|= (uint32_t)0x80U; // low side always OFF
       }
       else
       {
         u32Port1_IOCR0_Save|= (uint32_t)0xA8U; // low side always PWM
       }
       PORT1->IOCR0 = u32Port1_IOCR0_Save;

       //PWM cos
       u32Port2_IOCR0_Save = PORT2->IOCR0;
       u32Port2_IOCR0_Save &= (uint32_t)(0xFFFFFF00);
       if(u32PWM_DutyCycle_Cos <=u32CompareThreshold)
       {
    	   u32Port2_IOCR0_Save|= (uint32_t)0x80U; // low side always OFF
       }
       else
       {
         u32Port2_IOCR0_Save|= (uint32_t)0xA8U; // low side always PWM
       }
       PORT2->IOCR0 = u32Port2_IOCR0_Save;


   	   if(bDirection == 0)
       {
        s16Index++;
       }
       else
       {
        s16Index--;
       }

	   if(s16Index == NumOfMicroSteps)
       {
         s16Index = 0;
       }

	   if(s16Index ==  -1)
       {
         s16Index = NumOfMicroSteps-1;
       }

	  //u32RevolutionStepCounter++;
	  if(u32RevolutionStepCounter > u32RevolutionStepsLimit*NumOfMicroSteps)
	  {
		u32RevolutionStepCounter = 0;
		if(bDirection == 0)
		{
	      bDirection = 1;
	      s16Index = NumOfMicroSteps-1;
		}
	    else
		{
	      bDirection = 0;
	      s16Index = 0;
		}
	  }

      break;
    }

    case 6://Microstep sin cos
     {
       //setvalue for Sin and Cos current control
       u32PWM_DutyCycle_Sin = (uint32_t)(((uint32_t)atMicroStepSinCosTable[s16Index].u16SinMod * (uint32_t)u16ADCResult_Poti_akt) >> 12U);
  	   u32PWM_DutyCycle_Cos = (uint32_t)(((uint32_t)atMicroStepSinCosTable[s16Index].u16CosMod * (uint32_t)u16ADCResult_Poti_akt) >> 12U);

       PWM_CCU4_SetDutyCycle(&PWMSinGen,u32PWM_DutyCycle_Sin);
       PWM_CCU4_SetDutyCycle(&PWMCosGen,u32PWM_DutyCycle_Cos);


       // direction sin
   	   //0x1U:     set P0.12 to high => L/R-Dir Control = 1 => pos Current in SIN coil
   	   //0x10000U: set P0.12 to low => L/R-Dir Control = 1 => pos Current in SIN coil
   	   PORT0->OMR = ((uint32_t)atMicroStepSinCosTable[s16Index].u16DirCtrl_Sin << 12);

   	   // direction cos
   	   //0x1U:     set P0.8 to high => L/R-Dir Control = 1 => pos Current in COS coil
   	   //0x10000U: set P0.8 to low => L/R-Dir Control = 1 => pos Current in COS coil
   	   PORT0->OMR = ((uint32_t)atMicroStepSinCosTable[s16Index].u16DirCtrl_Cos << 8);

       //PWM sin
       u32Port1_IOCR0_Save = PORT1->IOCR0;
       u32Port1_IOCR0_Save &= (uint32_t)(0xFFFFFF00);
       if(u32PWM_DutyCycle_Sin <= u32CompareThreshold)
       {
         u32Port1_IOCR0_Save|= (uint32_t)0x80U; // low side always OFF
       }
       else
       {
         u32Port1_IOCR0_Save|= (uint32_t)0xA8U; // low side always PWM
       }
       PORT1->IOCR0 = u32Port1_IOCR0_Save;

       //PWM cos
       u32Port2_IOCR0_Save = PORT2->IOCR0;
       u32Port2_IOCR0_Save &= (uint32_t)(0xFFFFFF00);
       if(u32PWM_DutyCycle_Cos <=u32CompareThreshold)
       {
    	   u32Port2_IOCR0_Save|= (uint32_t)0x80U; // low side always OFF
       }
       else
       {
         u32Port2_IOCR0_Save|= (uint32_t)0xA8U; // low side always PWM
       }
       PORT2->IOCR0 = u32Port2_IOCR0_Save;


   	   if(bDirection == 0)
       {
        s16Index++;
       }
       else
       {
        s16Index--;
       }

	   if(s16Index == NumOfMicroSteps)
       {
         s16Index = 0;
       }

	   if(s16Index ==  -1)
       {
         s16Index = NumOfMicroSteps-1;
       }

	  //u32RevolutionStepCounter++;
	  if(u32RevolutionStepCounter > u32RevolutionStepsLimit*NumOfMicroSteps)
	  {
		u32RevolutionStepCounter = 0;
		if(bDirection == 0)
		{
	      bDirection = 1;
	      s16Index = NumOfMicroSteps-1;
		}
	    else
		{
	      bDirection = 0;
	      s16Index = 0;
		}
	  }

      break;
    }

    default://Stop Motor
    {
      //setvalue for Sin and Cos current control
      PWM_CCU4_SetDutyCycle(&PWMSinGen,0);
      PWM_CCU4_SetDutyCycle(&PWMCosGen,0);
      DIGITAL_IO_SetOutputHigh(&DisablePwrBridges); // Power Bridges OFF
      break;
     }
    }//end switch
  }
  else
  {//not enabled
	  //setvalue for Sin and Cos current control
	  PWM_CCU4_SetDutyCycle(&PWMSinGen,0);
      PWM_CCU4_SetDutyCycle(&PWMCosGen,0);
      DIGITAL_IO_SetOutputHigh(&DisablePwrBridges); // Power Bridges OFF
      s16Index = 0;
  }



  DIGITAL_IO_SetOutputLow(&TestOutput);
}//end ISR





void TxFinished(void)
{
	u32TxFinishedCounter++;
}

void ISR_UART_RxFinished(void)
{
	u32RxFinishedCounter++;
}





