#include "power2dspgenerator.h"

QStringList Power2DspGenerator::mmodeAdd4LW(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList, quint32 sampleCountPerSignalPeriod)
{
    QStringList dspCmdList;
    dspCmdList.append("ACTIVATECHAIN(1,0x0110)");
    dspCmdList.append(QString("TESTVCSKIPEQ(MMODE,%1)").arg(dspSelectCode));
    dspCmdList.append("DEACTIVATECHAIN(1,0x0110)");
    dspCmdList.append("STARTCHAIN(0,1,0x0110)"); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start

    // our first measuring system
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(measChannelPairList[0].voltageChannel));
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[0].currentChannel));
    dspCmdList.append(QString("MULCCC(MEASSIGNAL1,MEASSIGNAL2,MEASSIGNAL1)"));
    dspCmdList.append(QString("INTEGRALPOS(%1,MEASSIGNAL1,VALPOWER)").arg(sampleCountPerSignalPeriod));
    dspCmdList.append(QString("INTEGRALNEG(%1,MEASSIGNAL1,VALPOWER+1)").arg(sampleCountPerSignalPeriod));
    dspCmdList.append(QString("INTEGRAL(%1,MEASSIGNAL1,VALPOWER+2)").arg(sampleCountPerSignalPeriod));

    // our second measuring system
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(measChannelPairList[1].voltageChannel));
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[1].currentChannel));
    dspCmdList.append(QString("MULCCC(MEASSIGNAL1,MEASSIGNAL2,MEASSIGNAL1)"));
    dspCmdList.append(QString("INTEGRALPOS(%1,MEASSIGNAL1,VALPOWER+3)").arg(sampleCountPerSignalPeriod));
    dspCmdList.append(QString("INTEGRALNEG(%1,MEASSIGNAL1,VALPOWER+4)").arg(sampleCountPerSignalPeriod));
    dspCmdList.append(QString("INTEGRAL(%1,MEASSIGNAL1,VALPOWER+5)").arg(sampleCountPerSignalPeriod));

    // our third measuring system
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(measChannelPairList[2].voltageChannel));
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[2].currentChannel));
    dspCmdList.append(QString("MULCCC(MEASSIGNAL1,MEASSIGNAL2,MEASSIGNAL1)"));
    dspCmdList.append(QString("INTEGRALPOS(%1,MEASSIGNAL1,VALPOWER+6)").arg(sampleCountPerSignalPeriod));
    dspCmdList.append(QString("INTEGRALNEG(%1,MEASSIGNAL1,VALPOWER+7)").arg(sampleCountPerSignalPeriod));
    dspCmdList.append(QString("INTEGRAL(%1,MEASSIGNAL1,VALPOWER+8)").arg(sampleCountPerSignalPeriod));

    dspCmdList.append("STOPCHAIN(1,0x0110)");
    return dspCmdList;
}
