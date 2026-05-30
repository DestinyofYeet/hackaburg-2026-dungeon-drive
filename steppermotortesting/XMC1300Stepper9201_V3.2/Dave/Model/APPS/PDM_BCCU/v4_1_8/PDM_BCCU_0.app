<?xml version="1.0" encoding="ASCII"?>
<ResourceModel:App xmi:version="2.0" xmlns:xmi="http://www.omg.org/XMI" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ResourceModel="http://www.infineon.com/Davex/Resource.ecore" name="PDM_BCCU" URI="http://resources/4.1.8/app/PDM_BCCU/0" description="PDM_BCCU APP configures the Channel registers &#xA;of the Brightness and Color Control Unit (BCCU)." mode="NOTSHARABLE" version="4.1.8" minDaveVersion="4.0.0" instanceLabel="PDM_BCCU_0" appLabel="">
  <properties provideInit="true"/>
  <virtualSignals name="pdm_output" URI="http://resources/4.1.8/app/PDM_BCCU/0/vs_pdmbccu_out" hwSignal="out" hwResource="//@hwResources.0" visible="true"/>
  <virtualSignals name="pdm_output_pin" URI="http://resources/4.1.8/app/PDM_BCCU/0/vs_pdmbccu_pad" hwSignal="pad" hwResource="//@hwResources.1" required="false"/>
  <virtualSignals name="dimming_level_input" URI="http://resources/4.1.8/app/PDM_BCCU/0/vs_pdmbccu_dimin" hwSignal="dimin" hwResource="//@hwResources.0" visible="true"/>
  <virtualSignals name="gating_input" URI="http://resources/4.1.8/app/PDM_BCCU/0/vs_pdmbccu_in" hwSignal="in" hwResource="//@hwResources.0" visible="true"/>
  <virtualSignals name="PDM Output pin_signal" URI="http://resources/4.1.8/app/PDM_BCCU/0/__pin_vs_pdmbccu_pad" hwSignal="pin" hwResource="//@hwResources.2"/>
  <requiredApps URI="http://resources/4.1.8/app/PDM_BCCU/0/appres_global_bccu" requiredAppName="GLOBAL_BCCU" requiringMode="SHARABLE">
    <downwardMapList xsi:type="ResourceModel:App" href="../../GLOBAL_BCCU/v4_1_6/GLOBAL_BCCU_0.app#/"/>
  </requiredApps>
  <hwResources name="BCCUChannel" URI="http://resources/4.1.8/app/PDM_BCCU/0/hwres_bccu" resourceGroupUri="peripheral/bccu/*/channel/*" mResGrpUri="peripheral/bccu/*/channel/*">
    <downwardMapList xsi:type="ResourceModel:ResourceGroup" href="../../../HW_RESOURCES/BCCU0/BCCU0_0.dd#//@provided.15"/>
  </hwResources>
  <hwResources name="PDM Output pin" URI="http://resources/4.1.8/app/PDM_BCCU/0/hwres_io" resourceGroupUri="" required="false" mResGrpUri="port/p/*/pad/*"/>
  <hwResources name="PDM Output pin" URI="http://resources/4.1.8/app/PDM_BCCU/0/__pin_hwres_io" resourceGroupUri="" required="false" mResGrpUri="devicepackage/0/*"/>
  <connections URI="http://resources/4.1.8/app/PDM_BCCU/0/http://resources/4.1.8/app/PDM_BCCU/0/vs_pdmbccu_out/http://resources/4.1.8/app/PDM_BCCU/0/vs_pdmbccu_pad" systemDefined="true" sourceSignal="pdm_output" targetSignal="pdm_output_pin" required="false" srcVirtualSignal="//@virtualSignals.0" targetVirtualSignal="//@virtualSignals.1"/>
  <connections URI="http://resources/4.1.8/app/PDM_BCCU/0/http://resources/4.1.8/app/PDM_BCCU/0/vs_pdmbccu_pad/http://resources/4.1.8/app/PDM_BCCU/0/__pin_vs_pdmbccu_pad" systemDefined="true" sourceSignal="pdm_output_pin" targetSignal="PDM Output pin_signal" srcVirtualSignal="//@virtualSignals.1" targetVirtualSignal="//@virtualSignals.4"/>
  <connections URI="http://resources/4.1.8/app/PDM_BCCU/0/http://resources/4.1.8/app/PDM_BCCU/0/__pin_vs_pdmbccu_pad/http://resources/4.1.8/app/PDM_BCCU/0/vs_pdmbccu_pad" systemDefined="true" sourceSignal="PDM Output pin_signal" targetSignal="pdm_output_pin" srcVirtualSignal="//@virtualSignals.4" targetVirtualSignal="//@virtualSignals.1"/>
</ResourceModel:App>
