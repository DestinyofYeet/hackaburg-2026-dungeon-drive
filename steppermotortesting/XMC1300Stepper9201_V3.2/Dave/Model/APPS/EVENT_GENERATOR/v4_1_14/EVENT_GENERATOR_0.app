<?xml version="1.0" encoding="ASCII"?>
<ResourceModel:App xmi:version="2.0" xmlns:xmi="http://www.omg.org/XMI" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ResourceModel="http://www.infineon.com/Davex/Resource.ecore" name="EVENT_GENERATOR" URI="http://resources/4.1.14/app/EVENT_GENERATOR/0" description="Provides the configuration for Output Gating Unit of ERU" version="4.1.14" minDaveVersion="4.0.0" instanceLabel="ERU_SwitchOffOutSin" appLabel="">
  <properties provideInit="true" sharable="true"/>
  <virtualSignals name="pattern" URI="http://resources/4.1.14/app/EVENT_GENERATOR/0/vs_event_generator_pdin" hwSignal="pdin" hwResource="//@hwResources.0" visible="true">
    <upwardMapList xsi:type="ResourceModel:Connections" href="../../EVENT_DETECTOR/v4_0_6/EVENT_DETECTOR_0.app#//@connections.0"/>
  </virtualSignals>
  <virtualSignals name="trigger_in" URI="http://resources/4.1.14/app/EVENT_GENERATOR/0/vs_event_generator_trin" hwSignal="trin" hwResource="//@hwResources.0" visible="true">
    <upwardMapList xsi:type="ResourceModel:Connections" href="../../EVENT_DETECTOR/v4_0_6/EVENT_DETECTOR_0.app#//@connections.1"/>
  </virtualSignals>
  <virtualSignals name="pdout" URI="http://resources/4.1.14/app/EVENT_GENERATOR/0/vs_event_generator_pdout" hwSignal="pdout" hwResource="//@hwResources.0" visible="true"/>
  <virtualSignals name="gout" URI="http://resources/4.1.14/app/EVENT_GENERATOR/0/vs_event_generator_gout" hwSignal="gout" hwResource="//@hwResources.0" visible="true"/>
  <virtualSignals name="iout" URI="http://resources/4.1.14/app/EVENT_GENERATOR/0/vs_event_generator_iout" hwSignal="iout" hwResource="//@hwResources.0" visible="true"/>
  <hwResources name="ogu" URI="http://resources/4.1.14/app/EVENT_GENERATOR/0/hwres_eru_ogu" resourceGroupUri="peripheral/eru/*/ogu/*" mResGrpUri="peripheral/eru/*/ogu/*">
    <downwardMapList xsi:type="ResourceModel:ResourceGroup" href="../../../HW_RESOURCES/ERU0/ERU0_0.dd#//@provided.39"/>
  </hwResources>
  <connections URI="http://resources/4.1.14/app/EVENT_GENERATOR/0/http://resources/4.1.14/app/EVENT_GENERATOR/0/vs_event_generator_pdout/http://resources/4.1.20/app/PWM_CCU8/0/vs_ccu8_cc8_gp0_unsync" sourceSignal="pdout" targetSignal="ext_event0" srcVirtualSignal="//@virtualSignals.2">
    <downwardMapList xsi:type="ResourceModel:VirtualSignal" href="../../PWM_CCU8/v4_1_20/PWM_CCU8_0.app#//@virtualSignals.14"/>
    <targetVirtualSignal href="../../PWM_CCU8/v4_1_20/PWM_CCU8_0.app#//@virtualSignals.14"/>
  </connections>
  <connections URI="http://resources/4.1.14/app/EVENT_GENERATOR/0/http://resources/4.1.14/app/EVENT_GENERATOR/0/vs_event_generator_iout/http://resources/4.0.18/app/ADC_SCAN/1/vs_adc_scan_greqtr0sel" sourceSignal="iout" targetSignal="trigger_input" srcVirtualSignal="//@virtualSignals.4">
    <downwardMapList xsi:type="ResourceModel:VirtualSignal" href="../../ADC_SCAN/v4_0_18/ADC_SCAN_1.app#//@virtualSignals.4"/>
    <targetVirtualSignal href="../../ADC_SCAN/v4_0_18/ADC_SCAN_1.app#//@virtualSignals.4"/>
  </connections>
</ResourceModel:App>
