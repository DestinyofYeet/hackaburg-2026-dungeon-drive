<?xml version="1.0" encoding="ASCII"?>
<ResourceModel:App xmi:version="2.0" xmlns:xmi="http://www.omg.org/XMI" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:ResourceModel="http://www.infineon.com/Davex/Resource.ecore" name="GLOBAL_ACMP" URI="http://resources/4.0.6/app/GLOBAL_ACMP/0" description="Initializes analog comparator power mode of operation" version="4.0.6" minDaveVersion="4.0.0" instanceLabel="GLOBAL_ACMP_0" appLabel="">
  <upwardMapList xsi:type="ResourceModel:RequiredApp" href="../../ACMP_CONFIG/v4_1_2/ACMP_CONFIG_0.app#//@requiredApps.0"/>
  <upwardMapList xsi:type="ResourceModel:RequiredApp" href="../../ACMP_CONFIG/v4_1_2/ACMP_CONFIG_1.app#//@requiredApps.0"/>
  <properties singleton="true" provideInit="true" sharable="true"/>
  <hwResources name="low power" URI="http://resources/4.0.6/app/GLOBAL_ACMP/0/hwres_acmp_low_power_mode" resourceGroupUri="peripheral/scu/*/cmpcu/acmp_powermode" mResGrpUri="peripheral/scu/*/cmpcu/acmp_powermode">
    <downwardMapList xsi:type="ResourceModel:ResourceGroup" href="../../../HW_RESOURCES/SCU/SCU_0.dd#//@provided.5"/>
  </hwResources>
</ResourceModel:App>
