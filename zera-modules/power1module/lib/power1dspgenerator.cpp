#include "power1dspgenerator.h"
#include "inttohexstringconvert.h"

QStringList Power1DspGenerator::mmodeAdd4LW(int dspSelectCode)
{
}

QStringList Power1DspGenerator::mmodeAdd4LB(int dspSelectCode)
{
}

QStringList Power1DspGenerator::mmodeAdd4LBK(int dspSelectCode)
{
}

QStringList Power1DspGenerator::mmodeAdd4LS(int dspSelectCode)
{

}

QStringList Power1DspGenerator::mmodeAdd4LSg(int dspSelectCode)
{

}

QStringList Power1DspGenerator::mmodeAdd3LW(int dspSelectCode)
{

}

QStringList Power1DspGenerator::mmodeAdd3LB(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList)
{
    QStringList dspCmdList;
    dspCmdList.append("ACTIVATECHAIN(1,0x0116)");
    dspCmdList.append(QString("TESTVCSKIPEQ(MMODE,%1)").arg(dspSelectCode));
    dspCmdList.append("DEACTIVATECHAIN(1,0x0116)");
    dspCmdList.append("STARTCHAIN(0,1,0x0116)"); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start

    // our first measuring system
    dspCmdList.append(QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL1)")
                          .arg(measChannelPairList[0].voltageChannel)
                          .arg(measChannelPairList[1].voltageChannel));
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[0].currentChannel));
    dspCmdList.append(QString("DFT(1,MEASSIGNAL1,TEMP1)"));
    dspCmdList.append(QString("DFT(1,MEASSIGNAL2,TEMP2)"));
    dspCmdList.append(QString("MULVVV(TEMP1,TEMP2+1,VALPQS)"));
    dspCmdList.append(QString("MULVVV(TEMP2,TEMP1+1,TEMP1)"));
    dspCmdList.append(QString("SUBVVV(TEMP1,VALPQS,VALPQS)"));
    dspCmdList.append(QString("MULVVV(FAK,VALPQS,VALPQS)"));

    //dspCmdList.append("ROTATE(MEASSIGNAL2,270.0)");
    //dspCmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS)");

    // our second measuring system
    dspCmdList.append("SETVAL(VALPQS+1,0.0)"); // is 0 output

    // our third measuring system
    dspCmdList.append(QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL1)")
                          .arg(measChannelPairList[2].voltageChannel)
                          .arg(measChannelPairList[1].voltageChannel));
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[2].currentChannel));
    dspCmdList.append(QString("DFT(1,MEASSIGNAL1,TEMP1)"));
    dspCmdList.append(QString("DFT(1,MEASSIGNAL2,TEMP2)"));
    dspCmdList.append(QString("MULVVV(TEMP1,TEMP2+1,VALPQS+2)"));
    dspCmdList.append(QString("MULVVV(TEMP2,TEMP1+1,TEMP1)"));
    dspCmdList.append(QString("SUBVVV(TEMP1,VALPQS+2,VALPQS+2)"));
    dspCmdList.append(QString("MULVVV(FAK,VALPQS+2,VALPQS+2)"));

    //dspCmdList.append("ROTATE(MEASSIGNAL2,270.0)");
    //dspCmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS+2)");

    dspCmdList.append("STOPCHAIN(1,0x0116)");
    return dspCmdList;
}

QStringList Power1DspGenerator::mmodeAdd2LW(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList, int idx2WireMeasSystem)
{
    QStringList dspCmdList;
    dspCmdList.append("ACTIVATECHAIN(1,0x0117)");
    dspCmdList.append(QString("TESTVCSKIPEQ(MMODE,%1)").arg(dspSelectCode));
    dspCmdList.append("DEACTIVATECHAIN(1,0x0117)");
    dspCmdList.append("STARTCHAIN(0,1,0x0117)"); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start

    // first we set all our actual values to 0
    dspCmdList.append("SETVAL(VALPQS,0.0)");
    dspCmdList.append("SETVAL(VALPQS+1,0.0)");
    dspCmdList.append("SETVAL(VALPQS+2,0.0)");

    // and then we compute the 2 wire power values for the selected system
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(measChannelPairList[idx2WireMeasSystem].voltageChannel));
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[idx2WireMeasSystem].currentChannel));
    dspCmdList.append(QString("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS+%1)").arg(idx2WireMeasSystem));

    dspCmdList.append("STOPCHAIN(1,0x0117)");
    return dspCmdList;
}

QStringList Power1DspGenerator::mmodeAdd2LB(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList, int idx2WireMeasSystem)
{
    QStringList dspCmdList;
    dspCmdList.append("ACTIVATECHAIN(1,0x0118)");
    dspCmdList.append(QString("TESTVCSKIPEQ(MMODE,%1)").arg(dspSelectCode));
    dspCmdList.append("DEACTIVATECHAIN(1,0x0118)");
    dspCmdList.append("STARTCHAIN(0,1,0x0118)"); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start

    // first we set all our actual values to 0
    dspCmdList.append("SETVAL(VALPQS,0.0)");
    dspCmdList.append("SETVAL(VALPQS+1,0.0)");
    dspCmdList.append("SETVAL(VALPQS+2,0.0)");

    // and then we compute the 2 wire power values for the selected system
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(measChannelPairList[idx2WireMeasSystem].voltageChannel));
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[idx2WireMeasSystem].currentChannel));

    dspCmdList.append(QString("DFT(1,MEASSIGNAL1,TEMP1)"));
    dspCmdList.append(QString("DFT(1,MEASSIGNAL2,TEMP2)"));
    dspCmdList.append(QString("MULVVV(TEMP1,TEMP2+1,VALPQS+%1)").arg(idx2WireMeasSystem));
    dspCmdList.append(QString("MULVVV(TEMP2,TEMP1+1,TEMP1)"));
    dspCmdList.append(QString("SUBVVV(TEMP1,VALPQS+%1,VALPQS+%2)").arg(idx2WireMeasSystem).arg(idx2WireMeasSystem));
    dspCmdList.append(QString("MULVVV(FAK,VALPQS+%1,VALPQS+%2)").arg(idx2WireMeasSystem).arg(idx2WireMeasSystem));

    //dspCmdList.append("ROTATE(MEASSIGNAL2,270.0)");
    //dspCmdList.append(QString("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS+%1)").arg(m_idx2WireMeasSystem));

    dspCmdList.append("STOPCHAIN(1,0x0118)");
    return dspCmdList;
}

QStringList Power1DspGenerator::mmodeAdd2LS(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList, int idx2WireMeasSystem)
{
    QStringList dspCmdList;
    dspCmdList.append("ACTIVATECHAIN(1,0x0119)");
    dspCmdList.append(QString("TESTVCSKIPEQ(MMODE,%1)").arg(dspSelectCode));
    dspCmdList.append("DEACTIVATECHAIN(1,0x0119)");
    dspCmdList.append("STARTCHAIN(0,1,0x0119)"); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start

    // first we set all our actual values to 0
    dspCmdList.append("SETVAL(VALPQS,0.0)");
    dspCmdList.append("SETVAL(VALPQS+1,0.0)");
    dspCmdList.append("SETVAL(VALPQS+2,0.0)");

    // and then we compute the 2 wire power values for the selected system
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(measChannelPairList[idx2WireMeasSystem].voltageChannel));
    dspCmdList.append("RMS(MEASSIGNAL1,TEMP1)");
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(measChannelPairList[idx2WireMeasSystem].currentChannel));
    dspCmdList.append("RMS(MEASSIGNAL1,TEMP2)");
    dspCmdList.append(QString("MULVVV(TEMP1,TEMP2,VALPQS+%1)").arg(idx2WireMeasSystem));

    dspCmdList.append("STOPCHAIN(1,0x0119)");
    return dspCmdList;
}

QStringList Power1DspGenerator::mmodeAdd2LSg(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList, int idx2WireMeasSystem)
{
    QStringList dspCmdList;
    dspCmdList.append("ACTIVATECHAIN(1,0x0120)");
    dspCmdList.append(QString("TESTVCSKIPEQ(MMODE,%1)").arg(dspSelectCode));
    dspCmdList.append("DEACTIVATECHAIN(1,0x0120)");
    dspCmdList.append("STARTCHAIN(0,1,0x0120)"); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start

    // first we set all our actual values to 0
    dspCmdList.append("SETVAL(VALPQS,0.0)");
    dspCmdList.append("SETVAL(VALPQS+1,0.0)");
    dspCmdList.append("SETVAL(VALPQS+2,0.0)");

    // and then we compute the 2 wire power values for the selected system
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(measChannelPairList[idx2WireMeasSystem].voltageChannel));
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[idx2WireMeasSystem].currentChannel));
    dspCmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,TEMP1)"); // P
    dspCmdList.append("ROTATE(MEASSIGNAL2,270.0)");
    dspCmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,TEMP2)"); // Q
    dspCmdList.append(QString("ADDVVG(TEMP1,TEMP2,VALPQS+%1)").arg(idx2WireMeasSystem));

    dspCmdList.append("STOPCHAIN(1,0x0120)");
    return dspCmdList;
}

QStringList Power1DspGenerator::mmodeAddXLW(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList)
{
    QStringList cmdList;
    for(int phase=0; phase<measChannelPairList.count(); phase++) {
        // reset power valuse
        QString strChains =  IntToHexStringConvert::convert(0x0121 + phase);
        cmdList.append(QString("ACTIVATECHAIN(1,%1)").arg(strChains));
        cmdList.append(QString("TESTVCSKIPEQ(MMODE,%1)").arg(dspSelectCode));
        cmdList.append(QString("DEACTIVATECHAIN(1,%1)").arg(strChains));
        cmdList.append(QString("TESTVCSKIPEQ(XMMODEPHASE%1,0)").arg(phase));
        cmdList.append(QString("DEACTIVATECHAIN(1,%1)").arg(strChains));
        cmdList.append(QString("STARTCHAIN(0,1,%1)").arg(strChains)); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        cmdList.append(QString("SETVAL(VALPQS+%1,0.0)").arg(phase));
        cmdList.append(QString("STOPCHAIN(1,%1)").arg(strChains)); // ende prozessnr., hauptkette 1 subkette 1
        // calc power values
        strChains =  IntToHexStringConvert::convert(0x0121 + phase<measChannelPairList.count() + phase);
        cmdList.append(QString("ACTIVATECHAIN(1,%1)").arg(strChains));
        cmdList.append(QString("TESTVCSKIPEQ(MMODE,%1)").arg(dspSelectCode));
        cmdList.append(QString("DEACTIVATECHAIN(1,%1)").arg(strChains));
        cmdList.append(QString("TESTVCSKIPEQ(XMMODEPHASE%1,1)").arg(phase));
        cmdList.append(QString("DEACTIVATECHAIN(1,%1)").arg(strChains));
        cmdList.append(QString("STARTCHAIN(0,1,%1)").arg(strChains)); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        cmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(measChannelPairList[phase].voltageChannel));
        cmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[phase].currentChannel));
        cmdList.append(QString("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS+%1)").arg(phase));
        cmdList.append(QString("STOPCHAIN(1,%1)").arg(strChains)); // ende prozessnr., hauptkette 1 subkette 1
    }
    return cmdList;
}

QStringList Power1DspGenerator::mmodeAddMQREF(int dspSelectCode)
{
    QStringList dspCmdList;
    dspCmdList.append("ACTIVATECHAIN(1,0x0127)");
    dspCmdList.append(QString("TESTVCSKIPEQ(MMODE,%1)").arg(dspSelectCode));
    dspCmdList.append("DEACTIVATECHAIN(1,0x0127)");
    dspCmdList.append("STARTCHAIN(0,1,0x0127)"); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start

    // we simply set all our actual values to nominal power
    dspCmdList.append("COPYVAL(NOMPOWER,VALPQS)");
    dspCmdList.append("COPYVAL(NOMPOWER,VALPQS+1)");
    dspCmdList.append("COPYVAL(NOMPOWER,VALPQS+2)");

    dspCmdList.append("STOPCHAIN(1,0x0127)");
    return dspCmdList;
}
