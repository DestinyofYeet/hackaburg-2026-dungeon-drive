<?xml version="1.0" encoding="ASCII"?>
<ResourceModel:App xmi:version="2.0" xmlns:xmi="http://www.omg.org/XMI" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ResourceModel="http://www.infineon.com/Davex/Resource.ecore" name="GLOBAL_BCCU" URI="http://resources/4.1.6/app/GLOBAL_BCCU/0" description="GLOBAL_BCCU APP configures the global registers of the &#xA;Brightness and Color Control Unit (BCCU). &#xA;The configured parameters are clock frequencies, &#xA;trap functionality, triggering, channel passive levels &#xA;and interrupts. The APP provides dynamic APIs &#xA;to high level APPs such as event flag &#xA;configuration, interrupt configuration and trap configuration." version="4.1.6" minDaveVersion="4.0.0" instanceLabel="GLOBAL_BCCU_0" appLabel="">
  <upwardMapList xsi:type="ResourceModel:RequiredApp" href="../../PDM_BCCU/v4_1_8/PDM_BCCU_0.app#//@requiredApps.0"/>
  <properties singleton="true" provideInit="true" sharable="true"/>
  <virtualSignals name="event_trap_output" URI="http://resources/4.1.6/app/GLOBAL_BCCU/0/vs_trap_out" hwSignal="trap_out" hwResource="//@hwResources.0" visible="true"/>
  <virtualSignals name="trap_input" URI="http://resources/4.1.6/app/GLOBAL_BCCU/0/vs_trap_in" hwSignal="trap_in" hwResource="//@hwResources.0" visible="true">
    <upwardMapList xsi:type="ResourceModel:Connections" href="../../DIGITAL_IO/v4_0_16/DIGITAL_IO_12.app#//@connections.2"/>
  </virtualSignals>
  <virtualSignals name="event_trigout0 " URI="http://resources/4.1.6/app/GLOBAL_BCCU/0/vs_trigger_out0" hwSignal="trigger_out0" hwResource="//@hwResources.1" visible="true"/>
  <virtualSignals name="event_trigout1" URI="http://resources/4.1.6/app/GLOBAL_BCCU/0/vs_trigger_out1" hwSignal="trigger_out1" hwResource="//@hwResources.1" visible="true"/>
  <virtualSignals name="event_fifo_full" URI="http://resources/4.1.6/app/GLOBAL_BCCU/0/vs_event_full" hwSignal="event_full" hwResource="//@hwResources.2" visible="true"/>
  <virtualSignals name="event_fifo_empty" URI="http://resources/4.1.6/app/GLOBAL_BCCU/0/vs_event_empty" hwSignal="event_empty" hwResource="//@hwResources.2" visible="true"/>
  <virtualSignals name="global_dimming_level" URI="http://resources/4.1.6/app/GLOBAL_BCCU/0/vs_global_dimming_engine" hwSignal="globdimout" hwResource="//@hwResources.3" visible="true"/>
  <requiredApps URI="http://resources/4.1.6/app/GLOBAL_BCCU/0/appres_clock" requiredAppName="CLOCK_XMC1" requiringMode="SHARABLE">
    <downwardMapList xsi:type="ResourceModel:App" href="../../CLOCK_XMC1/v4_0_18/CLOCK_XMC1_0.app#/"/>
  </requiredApps>
  <hwResources name="Trap" URI="http://resources/4.1.6/app/GLOBAL_BCCU/0/hwres_trap" resourceGroupUri="peripheral/bccu/*/trap" mResGrpUri="peripheral/bccu/*/trap">
    <downwardMapList xsi:type="ResourceModel:ResourceGroup" href="../../../HW_RESOURCES/BCCU0/BCCU0_0.dd#//@provided.11"/>
  </hwResources>
  <hwResources name="Trigger" URI="http://resources/4.1.6/app/GLOBAL_BCCU/0/hwres_trigger" resourceGroupUri="peripheral/bccu/*/trigger" mResGrpUri="peripheral/bccu/*/trigger">
    <downwardMapList xsi:type="ResourceModel:ResourceGroup" href="../../../HW_RESOURCES/BCCU0/BCCU0_0.dd#//@provided.8"/>
  </hwResources>
  <hwResources name="Packer" URI="http://resources/4.1.6/app/GLOBAL_BCCU/0/hwres_packer_fifo_event" resourceGroupUri="peripheral/bccu/*/packer_fifo_event" mResGrpUri="peripheral/bccu/*/packer_fifo_event">
    <downwardMapList xsi:type="ResourceModel:ResourceGroup" href="../../../HW_RESOURCES/BCCU0/BCCU0_0.dd#//@provided.4"/>
  </hwResources>
  <hwResources name="Global" URI="http://resources/4.1.6/app/GLOBAL_BCCU/0/hwres_global" resourceGroupUri="peripheral/bccu/*/global" mResGrpUri="peripheral/bccu/*/global">
    <downwardMapList xsi:type="ResourceModel:ResourceGroup" href="../../../HW_RESOURCES/BCCU0/BCCU0_0.dd#//@provided.0"/>
  </hwResources>
  <connections URI="http://resources/4.1.6/app/GLOBAL_BCCU/0/http://resources/4.1.6/app/GLOBAL_BCCU/0/vs_trap_out/http://resources/4.0.8/app/INTERRUPT/1/vs_nvic_signal_in" sourceSignal="event_trap_output" targetSignal="sr_irq" srcVirtualSignal="//@virtualSignals.0">
    <downwardMapList xsi:type="ResourceModel:VirtualSignal" href="../../INTERRUPT/v4_0_8/INTERRUPT_1.app#//@virtualSignals.0"/>
    <targetVirtualSignal href="../../INTERRUPT/v4_0_8/INTERRUPT_1.app#//@virtualSignals.0"/>
  </connections>
</ResourceModel:App>
