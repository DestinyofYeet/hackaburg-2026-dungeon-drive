<?xml version="1.0" encoding="ASCII"?>
<ResourceModel:App xmi:version="2.0" xmlns:xmi="http://www.omg.org/XMI" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ResourceModel="http://www.infineon.com/Davex/Resource.ecore" name="ACMP_CONFIG" URI="http://resources/4.1.2/app/ACMP_CONFIG/0" description="Configures the properties of analog comparator peripheral" mode="NOTSHARABLE" version="4.1.2" minDaveVersion="4.0.0" instanceLabel="ACMPSin" appLabel="">
  <properties provideInit="true"/>
  <virtualSignals name="acmp_out" URI="http://resources/4.1.2/app/ACMP_CONFIG/0/vs_acmp_out" hwSignal="out" hwResource="//@hwResources.0" visible="true"/>
  <virtualSignals name="acmp_inn" URI="http://resources/4.1.2/app/ACMP_CONFIG/0/vs_acmp_inn" hwSignal="inn" hwResource="//@hwResources.0" visible="true"/>
  <virtualSignals name="acmp_inp" URI="http://resources/4.1.2/app/ACMP_CONFIG/0/vs_acmp_inp" hwSignal="inp" hwResource="//@hwResources.0" visible="true"/>
  <requiredApps URI="http://resources/4.1.2/app/ACMP_CONFIG/0/appres_global_acmp" requiredAppName="GLOBAL_ACMP" requiringMode="SHARABLE">
    <downwardMapList xsi:type="ResourceModel:App" href="../../GLOBAL_ACMP/v4_0_6/GLOBAL_ACMP_0.app#/"/>
  </requiredApps>
  <hwResources name="analog comparator" URI="http://resources/4.1.2/app/ACMP_CONFIG/0/hwres_acmp_slice" resourceGroupUri="peripheral/scu/0/cmpcu/anacmp/0" constraintType="GLOBAL_RESOURCE" mResGrpUri="peripheral/scu/*/cmpcu/anacmp/*">
    <downwardMapList xsi:type="ResourceModel:ResourceGroup" href="../../../HW_RESOURCES/SCU/SCU_0.dd#//@provided.9"/>
  </hwResources>
  <connections URI="http://resources/4.1.2/app/ACMP_CONFIG/0/http://resources/4.1.2/app/ACMP_CONFIG/0/vs_acmp_out/http://resources/4.0.6/app/EVENT_DETECTOR/0/vs_event_detector_input_A" sourceSignal="acmp_out" targetSignal="signal_a" srcVirtualSignal="//@virtualSignals.0">
    <downwardMapList xsi:type="ResourceModel:VirtualSignal" href="../../EVENT_DETECTOR/v4_0_6/EVENT_DETECTOR_0.app#//@virtualSignals.0"/>
    <targetVirtualSignal href="../../EVENT_DETECTOR/v4_0_6/EVENT_DETECTOR_0.app#//@virtualSignals.0"/>
  </connections>
</ResourceModel:App>
