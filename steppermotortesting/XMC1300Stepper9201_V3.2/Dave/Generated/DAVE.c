
/**
 * @cond
 ***********************************************************************************************************************
 *
 * Copyright (c) 2015, Infineon Technologies AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,are permitted provided that the
 * following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this list of conditions and the  following
 *   disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 *   following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 *   Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote
 *   products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT  OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * To improve the quality of the software, users are encouraged to share modifications, enhancements or bug fixes
 * with Infineon Technologies AG (dave@infineon.com).
 ***********************************************************************************************************************
 *
 * Change History
 * --------------
 *
 * 2014-06-16:
 *     - Initial version<br>
 * 2015-08-28:
 *     - Added CLOCK_XMC1_Init conditionally
 *
 * @endcond
 *
 */

/***********************************************************************************************************************
 * HEADER FILES
 **********************************************************************************************************************/
#include "DAVE.h"

/***********************************************************************************************************************
 * API IMPLEMENTATION
 **********************************************************************************************************************/

/*******************************************************************************
 * @brief This function initializes the APPs Init Functions.
 *
 * @param[in]  None
 *
 * @return  DAVE_STATUS_t <BR>
 ******************************************************************************/
DAVE_STATUS_t DAVE_Init(void)
{
  DAVE_STATUS_t init_status;
  
  init_status = DAVE_STATUS_SUCCESS;
     /** @Initialization of APPs Init Functions */
     init_status = (DAVE_STATUS_t)CLOCK_XMC1_Init(&CLOCK_XMC1_0);

  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance Input_DIS */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&Input_DIS); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance Input_DIR */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&Input_DIR); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of PWM_CCU4 APP instance PWMSinGen */
	 init_status = (DAVE_STATUS_t)PWM_CCU4_Init(&PWMSinGen); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of PWM_CCU4 APP instance PWMCosGen */
	 init_status = (DAVE_STATUS_t)PWM_CCU4_Init(&PWMCosGen); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of ACMP_CONFIG APP instance ACMPSin */
	 init_status = (DAVE_STATUS_t)ACMP_CONFIG_Init(&ACMPSin); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of ACMP_CONFIG APP instance ACMPCos */
	 init_status = (DAVE_STATUS_t)ACMP_CONFIG_Init(&ACMPCos); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance DisablePwrBridges */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&DisablePwrBridges); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance DIRSin */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&DIRSin); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance DIRCos */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&DIRCos); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of PWM_CCU8 APP instance PWMOutSin */
	 init_status = (DAVE_STATUS_t)PWM_CCU8_Init(&PWMOutSin); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of PWM_CCU8 APP instance PWMOutCos */
	 init_status = (DAVE_STATUS_t)PWM_CCU8_Init(&PWMOutCos); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of PWM_CCU4 APP instance NoiseBlankSin */
	 init_status = (DAVE_STATUS_t)PWM_CCU4_Init(&NoiseBlankSin); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of EVENT_DETECTOR APP instance CMPSin_AND_GateTimeSin */
	 init_status = (DAVE_STATUS_t)EVENT_DETECTOR_Init(&CMPSin_AND_GateTimeSin); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of EVENT_GENERATOR APP instance ERU_SwitchOffOutSin */
	 init_status = (DAVE_STATUS_t)EVENT_GENERATOR_Init(&ERU_SwitchOffOutSin); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of PWM_CCU4 APP instance NoiseBlankCos */
	 init_status = (DAVE_STATUS_t)PWM_CCU4_Init(&NoiseBlankCos); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of EVENT_GENERATOR APP instance ERU_SwitchOffOutCos */
	 init_status = (DAVE_STATUS_t)EVENT_GENERATOR_Init(&ERU_SwitchOffOutCos); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of EVENT_DETECTOR APP instance CMPSin_AND_GateTimeCos */
	 init_status = (DAVE_STATUS_t)EVENT_DETECTOR_Init(&CMPSin_AND_GateTimeCos); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance DigIn_GateTimeSin */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&DigIn_GateTimeSin); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance DigIn_GateTimeCos */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&DigIn_GateTimeCos); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance DigIn_StartGateTimeCos */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&DigIn_StartGateTimeCos); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance DigIn_StartGateTimeSin */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&DigIn_StartGateTimeSin); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance BridgeErrorSin */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&BridgeErrorSin); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance BridgeErrorCos */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&BridgeErrorCos); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of UART APP instance Com2Host */
	 init_status = (DAVE_STATUS_t)UART_Init(&Com2Host); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance Input_STEPviaBCCUTrap */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&Input_STEPviaBCCUTrap); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of ADC_MEASUREMENT_ADV APP instance ADC_MEASUREMENT_ADV_0 */
	 init_status = (DAVE_STATUS_t)ADC_MEASUREMENT_ADV_Init(&ADC_MEASUREMENT_ADV_0); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance TestOutput */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&TestOutput); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance DiagLED */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&DiagLED); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of PDM_BCCU APP instance PDM_BCCU_0 */
	 init_status = (DAVE_STATUS_t)PDM_BCCU_Init(&PDM_BCCU_0); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of INTERRUPT APP instance StepInterrupt */
	 init_status = (DAVE_STATUS_t)INTERRUPT_Init(&StepInterrupt); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of ADC_MEASUREMENT_ADV APP instance ADC_MEASUREMENT_ADV_1 */
	 init_status = (DAVE_STATUS_t)ADC_MEASUREMENT_ADV_Init(&ADC_MEASUREMENT_ADV_1); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of ADC_MEASUREMENT APP instance ADC_MEASUREMENT_0 */
	 init_status = (DAVE_STATUS_t)ADC_MEASUREMENT_Init(&ADC_MEASUREMENT_0); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of E_EEPROM_XMC1 APP instance E_EEPROM_XMC1_0 */
	 init_status = (DAVE_STATUS_t)E_EEPROM_XMC1_Init(&E_EEPROM_XMC1_0); 
   }  
  return init_status;
} /**  End of function DAVE_Init */

