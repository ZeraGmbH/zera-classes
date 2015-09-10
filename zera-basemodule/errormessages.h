#ifndef ERRORMESSAGES_H
#define ERRORMESSAGES_H

#define rmidentErrMSG "RESMAN ident error"
#define resourcetypeErrMsg "RESMAN resourcetype not avail"
#define resourceErrMsg "RESMAN resource not avail"
#define resourceInfoErrMsg "RESMAN resource Info error"
#define claimresourceErrMsg "RESMAN set resource failed"
#define setresourceErrMsg "RESMAN set resource failed"
#define freeresourceErrMsg "RESMAN free resource failed"

#define readdspchannelErrMsg "PCB dsp channel read failed"
#define readaliasErrMsg "PCB alias read failed"
#define readsamplerateErrMsg "PCB sample rate read failed"
#define readunitErrMsg "PCB unit read failed"
#define readrangelistErrMsg "PCB range list read failed"
#define readrangealiasErrMsg "PCB range alias read failed"
#define readrangetypeErrMsg "PCB range type read failed"
#define readrangeurvalueErrMsg "PCB range urvalue read failed"
#define readrangerejectionErrMsg "PCB range rejection read failed"
#define readrangeovrejectionErrMsg "PCB range overload rejection failed"
#define readrangeavailErrMsg "PCB range avail info read failed"
#define setRangeErrMsg "PCB set range failed"
#define getRangeErrMsg "PCB get range failed"
#define setMeasModeErrMsg "PCB set measuring mode failed"
#define readGainCorrErrMsg "PCB read gain correction failed"
#define readOffsetCorrErrMsg "PCB read offset correction failed"
#define readPhaseCorrErrMsg "PCB read phase correction failed"
#define readChannelStatusErrMsg "PCB read channel status failed"
#define resetChannelStatusErrMsg "PCB reset channel status failed"
#define readFormFactorErrMsg "PCB formfactor read failed"
#define registerpcbnotifierErrMsg "PCB register notifier failed"
#define unregisterpcbnotifierErrMsg "PCB unregister notifier failed"
#define setPllErrMsg "PCB set pll failed"
#define readmuxchannelErrMsg "PCB muxchannel read failed"
#define readrefconstantErrMsg "PCB reference constant read failed"
#define readadjstatusErrMsg "PCB adjustment status read failed"

#define readdspgaincorrErrMsg "DSP read gain correction failed"
#define readdspphasecorrErrMsg "DSP read phase correction failed"
#define readdspoffsetcorrErrMsg "DSP read offset corredction failed"
#define writedspgaincorrErrMsg "DSP write gain correction failed"
#define writedspphasecorrErrMsg "DSP write phase correction failed"
#define writedspoffsetcorrErrMsg "DSP write offset corredction failed"
#define dspvarlistwriteErrMsg "DSP write varlist failed"
#define dspcmdlistwriteErrMsg "DSP write cmdlist failed"
#define dspactiveErrMsg "DSP measure activation failed"
#define dspdeactiveErrMsg "DSP measure deactivation failed"
#define dataaquisitionErrMsg "DSP data acquisition failed"
#define writedspmemoryErrMsg "DSP memory write failed"
#define writesubdcErrMsg "DSP subdc write failed"

#define fetchsececalcunitErrMsg "SEC fetch ecalculator failed"
#define freesececalcunitErrMsg "SEC free ecalculator failed"
#define readsecregisterErrMsg "SEC read register failed"
#define writesecregisterErrMsg "SEC write register failed"
#define setsyncErrMsg "SEC set sync failed"
#define setmuxErrMsg "SEC set mux failed"
#define setcmdidErrMsg "SEC set cmdid failed"
#define startmeasErrMsg "SEC stop measure failed"
#define stopmeasErrMsg "SEC stop measure failed"

#define interfacejsonErrMsg "Interface JSON Document strange"
#define interfaceETHErrMsg "Ethernet interface listen failed"
#define interfaceSerialErrMsg "Serial interface not connected"

#endif // ERRORMESSAGES_H
