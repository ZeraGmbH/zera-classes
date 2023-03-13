#include "power1dspcmdgenerator.h"
#include "inttohexstringconvert.h"

QStringList Power1DspCmdGenerator::getCmdsMModeMQREF(int dspSelectCode)
{
    QStringList dspCmdList;
    dspCmdList.append("ACTIVATECHAIN(1,0x0110)");
    dspCmdList.append(QString("TESTVCSKIPEQ(MMODE,%1)").arg(dspSelectCode));
    dspCmdList.append("DEACTIVATECHAIN(1,0x0110)");
    dspCmdList.append("STARTCHAIN(0,1,0x0110)"); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start

    // we simply set all our actual values to nominal power
    dspCmdList.append("COPYVAL(NOMPOWER,VALPQS)");
    dspCmdList.append("COPYVAL(NOMPOWER,VALPQS+1)");
    dspCmdList.append("COPYVAL(NOMPOWER,VALPQS+2)");

    dspCmdList.append("STOPCHAIN(1,0x0110)");
    return dspCmdList;
}

QStringList Power1DspCmdGenerator::getCmdsMMode4LBK(int dspSelectCode, MeasSystemChannels measChannelPairList)
{
    QStringList dspCmdList;
    dspCmdList.append("ACTIVATECHAIN(1,0x0112)");
    dspCmdList.append(QString("TESTVCSKIPEQ(MMODE,%1)").arg(dspSelectCode));
    dspCmdList.append("DEACTIVATECHAIN(1,0x0112)");
    dspCmdList.append("STARTCHAIN(0,1,0x0112)"); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start

    // our first measuring system
    dspCmdList.append(QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL1)")
                          .arg(measChannelPairList[1].voltageChannel)
                          .arg(measChannelPairList[2].voltageChannel));
    dspCmdList.append("MULCV(MEASSIGNAL1,0.57735027)"); // we correct 1/sqrt(3)
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[0].currentChannel));

    dspCmdList.append(QString("DFT(1,MEASSIGNAL1,TEMP1)"));
    dspCmdList.append(QString("DFT(1,MEASSIGNAL2,TEMP2)"));
    dspCmdList.append(QString("MULVVV(TEMP1,TEMP2,VALPQS)"));
    dspCmdList.append(QString("MULVVV(TEMP2+1,TEMP1+1,TEMP1)"));
    dspCmdList.append(QString("ADDVVV(TEMP1,VALPQS,VALPQS)"));
    dspCmdList.append(QString("MULVVV(FAK,VALPQS,VALPQS)"));

    //dspCmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS)");

    // our second measuring system
    dspCmdList.append(QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL1)")
                          .arg(measChannelPairList[2].voltageChannel)
                          .arg(measChannelPairList[0].voltageChannel));
    dspCmdList.append("MULCV(MEASSIGNAL1,0.57735027)"); // we correct 1/sqrt(3)
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[1].currentChannel));
    dspCmdList.append(QString("DFT(1,MEASSIGNAL1,TEMP1)"));
    dspCmdList.append(QString("DFT(1,MEASSIGNAL2,TEMP2)"));
    dspCmdList.append(QString("MULVVV(TEMP1,TEMP2,VALPQS+1)"));
    dspCmdList.append(QString("MULVVV(TEMP2+1,TEMP1+1,TEMP1)"));
    dspCmdList.append(QString("ADDVVV(TEMP1,VALPQS+1,VALPQS+1)"));
    dspCmdList.append(QString("MULVVV(FAK,VALPQS+1,VALPQS+1)"));

    //dspCmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS+1)");

    // our third measuring system
    dspCmdList.append(QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL1)")
                          .arg(measChannelPairList[0].voltageChannel)
                          .arg(measChannelPairList[1].voltageChannel));
    dspCmdList.append("MULCV(MEASSIGNAL1,0.57735027)"); // we correct 1/sqrt(3)
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[2].currentChannel));

    dspCmdList.append(QString("DFT(1,MEASSIGNAL1,TEMP1)"));
    dspCmdList.append(QString("DFT(1,MEASSIGNAL2,TEMP2)"));
    dspCmdList.append(QString("MULVVV(TEMP1,TEMP2,VALPQS+2)"));
    dspCmdList.append(QString("MULVVV(TEMP2+1,TEMP1+1,TEMP1)"));
    dspCmdList.append(QString("ADDVVV(TEMP1,VALPQS+2,VALPQS+2)"));
    dspCmdList.append(QString("MULVVV(FAK,VALPQS+2,VALPQS+2)"));

    //dspCmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS+2)");

    dspCmdList.append("STOPCHAIN(1,0x0112)");
    return dspCmdList;
}

QStringList Power1DspCmdGenerator::getCmdsMMode4LS(int dspSelectCode, MeasSystemChannels measChannelPairList)
{
    QStringList dspCmdList;
    dspCmdList.append("ACTIVATECHAIN(1,0x0113)");
    dspCmdList.append(QString("TESTVCSKIPEQ(MMODE,%1)").arg(dspSelectCode));
    dspCmdList.append("DEACTIVATECHAIN(1,0x0113)");
    dspCmdList.append("STARTCHAIN(0,1,0x0113)"); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start

    // our first measuring system
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(measChannelPairList[0].voltageChannel));
    dspCmdList.append("RMS(MEASSIGNAL1,TEMP1)");
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(measChannelPairList[0].currentChannel));
    dspCmdList.append("RMS(MEASSIGNAL1,TEMP2)");
    dspCmdList.append("MULVVV(TEMP1,TEMP2,VALPQS)");

    // our second measuring system
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(measChannelPairList[1].voltageChannel));
    dspCmdList.append("RMS(MEASSIGNAL1,TEMP1)");
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(measChannelPairList[1].currentChannel));
    dspCmdList.append("RMS(MEASSIGNAL1,TEMP2)");
    dspCmdList.append("MULVVV(TEMP1,TEMP2,VALPQS+1)");

    // our third measuring system
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(measChannelPairList[2].voltageChannel));
    dspCmdList.append("RMS(MEASSIGNAL1,TEMP1)");
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(measChannelPairList[2].currentChannel));
    dspCmdList.append("RMS(MEASSIGNAL1,TEMP2)");
    dspCmdList.append("MULVVV(TEMP1,TEMP2,VALPQS+2)");

    dspCmdList.append("STOPCHAIN(1,0x0113)");
    return dspCmdList;
}

QStringList Power1DspCmdGenerator::getCmdsMMode4LSg(int dspSelectCode, MeasSystemChannels measChannelPairList)
{
    QStringList dspCmdList;
    dspCmdList.append("ACTIVATECHAIN(1,0x0114)");
    dspCmdList.append(QString("TESTVCSKIPEQ(MMODE,%1)").arg(dspSelectCode));
    dspCmdList.append("DEACTIVATECHAIN(1,0x0114)");
    dspCmdList.append("STARTCHAIN(0,1,0x0114)"); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start

    // our first measuring system
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(measChannelPairList[0].voltageChannel));
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[0].currentChannel));
    dspCmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,TEMP1)"); // P
    dspCmdList.append("ROTATE(MEASSIGNAL2,270.0)");
    dspCmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,TEMP2)"); // Q
    dspCmdList.append("ADDVVG(TEMP1,TEMP2,VALPQS)"); // geometrical sum is our actual value

    // our second measuring system
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(measChannelPairList[1].voltageChannel));
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[1].currentChannel));
    dspCmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,TEMP1)"); // P
    dspCmdList.append("ROTATE(MEASSIGNAL2,270.0)");
    dspCmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,TEMP2)"); // Q
    dspCmdList.append("ADDVVG(TEMP1,TEMP2,VALPQS+1)"); // geometrical sum is our actual value

    // our third measuring system
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(measChannelPairList[2].voltageChannel));
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[2].currentChannel));
    dspCmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,TEMP1)"); // P
    dspCmdList.append("ROTATE(MEASSIGNAL2,270.0)");
    dspCmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,TEMP2)"); // Q
    dspCmdList.append("ADDVVG(TEMP1,TEMP2,VALPQS+2)"); // geometrical sum is our actual value

    dspCmdList.append("STOPCHAIN(1,0x0114)");
    return dspCmdList;
}

QStringList Power1DspCmdGenerator::getCmdsMMode3LW(int dspSelectCode, MeasSystemChannels measChannelPairList)
{
    QStringList dspCmdList;
    dspCmdList.append("ACTIVATECHAIN(1,0x0115)");
    dspCmdList.append(QString("TESTVCSKIPEQ(MMODE,%1)").arg(dspSelectCode));
    dspCmdList.append("DEACTIVATECHAIN(1,0x0115)");
    dspCmdList.append("STARTCHAIN(0,1,0x0115)"); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start

    // our first measuring system
    dspCmdList.append(QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL1)")
                          .arg(measChannelPairList[0].voltageChannel)
                          .arg(measChannelPairList[1].voltageChannel));
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[0].currentChannel));
    dspCmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS)");

    // our second measuring system
    dspCmdList.append("SETVAL(VALPQS+1,0.0)"); // is 0 output

    // our third measuring system
    dspCmdList.append(QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL1)")
                          .arg(measChannelPairList[2].voltageChannel)
                          .arg(measChannelPairList[1].voltageChannel));
    dspCmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[2].currentChannel));
    dspCmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS+2)");

    dspCmdList.append("STOPCHAIN(1,0x0115)");
    return dspCmdList;
}

QStringList Power1DspCmdGenerator::getCmdsMMode3LB(int dspSelectCode, MeasSystemChannels measChannelPairList)
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

QStringList Power1DspCmdGenerator::getCmdsMMode2LS(int dspSelectCode, MeasSystemChannels measChannelPairList, int idx2WireMeasSystem)
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

QStringList Power1DspCmdGenerator::getCmdsMMode2LSg(int dspSelectCode, MeasSystemChannels measChannelPairList, int idx2WireMeasSystem)
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

QStringList Power1DspCmdGenerator::getCmdsMModeXLW(int dspSelectCode, MeasSystemChannels measChannelPairList)
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
        strChains =  IntToHexStringConvert::convert(0x0121 + measChannelPairList.count() + phase);
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

QStringList Power1DspCmdGenerator::getCmdsMModeXLB(int dspSelectCode, MeasSystemChannels measChannelPairList)
{
    QStringList cmdList;
    for(int phase=0; phase<measChannelPairList.count(); phase++) {
        // reset power valuse
        QString strChains =  IntToHexStringConvert::convert(0x0127 + phase);
        cmdList.append(QString("ACTIVATECHAIN(1,%1)").arg(strChains));
        cmdList.append(QString("TESTVCSKIPEQ(MMODE,%1)").arg(dspSelectCode));
        cmdList.append(QString("DEACTIVATECHAIN(1,%1)").arg(strChains));
        cmdList.append(QString("TESTVCSKIPEQ(XMMODEPHASE%1,0)").arg(phase));
        cmdList.append(QString("DEACTIVATECHAIN(1,%1)").arg(strChains));
        cmdList.append(QString("STARTCHAIN(0,1,%1)").arg(strChains)); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        cmdList.append(QString("SETVAL(VALPQS+%1,0.0)").arg(phase));
        cmdList.append(QString("STOPCHAIN(1,%1)").arg(strChains)); // ende prozessnr., hauptkette 1 subkette 1
        // calc power values
        strChains =  IntToHexStringConvert::convert(0x0127 + measChannelPairList.count() + phase);
        cmdList.append(QString("ACTIVATECHAIN(1,%1)").arg(strChains));
        cmdList.append(QString("TESTVCSKIPEQ(MMODE,%1)").arg(dspSelectCode));
        cmdList.append(QString("DEACTIVATECHAIN(1,%1)").arg(strChains));
        cmdList.append(QString("TESTVCSKIPEQ(XMMODEPHASE%1,1)").arg(phase));
        cmdList.append(QString("DEACTIVATECHAIN(1,%1)").arg(strChains));
        cmdList.append(QString("STARTCHAIN(0,1,%1)").arg(strChains)); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        cmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(measChannelPairList[phase].voltageChannel));
        cmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[phase].currentChannel));

        cmdList.append(QString("DFT(1,MEASSIGNAL1,TEMP1)"));
        cmdList.append(QString("DFT(1,MEASSIGNAL2,TEMP2)"));
        cmdList.append(QString("MULVVV(TEMP1,TEMP2+1,VALPQS+%1)").arg(phase));
        cmdList.append(QString("MULVVV(TEMP2,TEMP1+1,TEMP1)"));
        cmdList.append(QString("SUBVVV(TEMP1,VALPQS+%1,VALPQS+%2)").arg(phase).arg(phase));
        cmdList.append(QString("MULVVV(FAK,VALPQS+%1,VALPQS+%2)").arg(phase).arg(phase));
        //cmdList.append("ROTATE(MEASSIGNAL2,270.0)");
        //cmdList.append(QString("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS+%1)").arg(m_idx2WireMeasSystem));

        cmdList.append(QString("STOPCHAIN(1,%1)").arg(strChains)); // ende prozessnr., hauptkette 1 subkette 1
    }
    return cmdList;
}

QStringList Power1DspCmdGenerator::getCmdsMModeXLS(int dspSelectCode, MeasSystemChannels measChannelPairList)
{
    QStringList cmdList;
    for(int phase=0; phase<measChannelPairList.count(); phase++) {
        // reset power valuse
        QString strChains =  IntToHexStringConvert::convert(0x0130 + phase);
        cmdList.append(QString("ACTIVATECHAIN(1,%1)").arg(strChains));
        cmdList.append(QString("TESTVCSKIPEQ(MMODE,%1)").arg(dspSelectCode));
        cmdList.append(QString("DEACTIVATECHAIN(1,%1)").arg(strChains));
        cmdList.append(QString("TESTVCSKIPEQ(XMMODEPHASE%1,0)").arg(phase));
        cmdList.append(QString("DEACTIVATECHAIN(1,%1)").arg(strChains));
        cmdList.append(QString("STARTCHAIN(0,1,%1)").arg(strChains)); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        cmdList.append(QString("SETVAL(VALPQS+%1,0.0)").arg(phase));
        cmdList.append(QString("STOPCHAIN(1,%1)").arg(strChains)); // ende prozessnr., hauptkette 1 subkette 1
        // calc power values
        strChains =  IntToHexStringConvert::convert(0x0130 + measChannelPairList.count() + phase);
        cmdList.append(QString("ACTIVATECHAIN(1,%1)").arg(strChains));
        cmdList.append(QString("TESTVCSKIPEQ(MMODE,%1)").arg(dspSelectCode));
        cmdList.append(QString("DEACTIVATECHAIN(1,%1)").arg(strChains));
        cmdList.append(QString("TESTVCSKIPEQ(XMMODEPHASE%1,1)").arg(phase));
        cmdList.append(QString("DEACTIVATECHAIN(1,%1)").arg(strChains));
        cmdList.append(QString("STARTCHAIN(0,1,%1)").arg(strChains)); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        cmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(measChannelPairList[phase].voltageChannel));
        cmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[phase].currentChannel));

        cmdList.append("RMS(MEASSIGNAL1,TEMP1)");
        cmdList.append("RMS(MEASSIGNAL2,TEMP2)");
        cmdList.append(QString("MULVVV(TEMP1,TEMP2,VALPQS+%1)").arg(phase));

        cmdList.append(QString("STOPCHAIN(1,%1)").arg(strChains)); // ende prozessnr., hauptkette 1 subkette 1
    }
    return cmdList;
}

QStringList Power1DspCmdGenerator::getCmdsMModeXLSg(int dspSelectCode, MeasSystemChannels measChannelPairList)
{
    QStringList cmdList;
    for(int phase=0; phase<measChannelPairList.count(); phase++) {
        // reset power valuse
        QString strChains =  IntToHexStringConvert::convert(0x0136 + phase);
        cmdList.append(QString("ACTIVATECHAIN(1,%1)").arg(strChains));
        cmdList.append(QString("TESTVCSKIPEQ(MMODE,%1)").arg(dspSelectCode));
        cmdList.append(QString("DEACTIVATECHAIN(1,%1)").arg(strChains));
        cmdList.append(QString("TESTVCSKIPEQ(XMMODEPHASE%1,0)").arg(phase));
        cmdList.append(QString("DEACTIVATECHAIN(1,%1)").arg(strChains));
        cmdList.append(QString("STARTCHAIN(0,1,%1)").arg(strChains)); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        cmdList.append(QString("SETVAL(VALPQS+%1,0.0)").arg(phase));
        cmdList.append(QString("STOPCHAIN(1,%1)").arg(strChains)); // ende prozessnr., hauptkette 1 subkette 1
        // calc power values
        strChains =  IntToHexStringConvert::convert(0x0136 + measChannelPairList.count() + phase);
        cmdList.append(QString("ACTIVATECHAIN(1,%1)").arg(strChains));
        cmdList.append(QString("TESTVCSKIPEQ(MMODE,%1)").arg(dspSelectCode));
        cmdList.append(QString("DEACTIVATECHAIN(1,%1)").arg(strChains));
        cmdList.append(QString("TESTVCSKIPEQ(XMMODEPHASE%1,1)").arg(phase));
        cmdList.append(QString("DEACTIVATECHAIN(1,%1)").arg(strChains));
        cmdList.append(QString("STARTCHAIN(0,1,%1)").arg(strChains)); // inaktiv, prozessnr. (dummy),hauptkette 1 subkette 1 start
        cmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(measChannelPairList[phase].voltageChannel));
        cmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[phase].currentChannel));

        cmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,TEMP1)"); // P
        cmdList.append("ROTATE(MEASSIGNAL2,270.0)");
        cmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,TEMP2)"); // Q
        cmdList.append(QString("ADDVVG(TEMP1,TEMP2,VALPQS+%1)").arg(phase));

        cmdList.append(QString("STOPCHAIN(1,%1)").arg(strChains)); // ende prozessnr., hauptkette 1 subkette 1
    }
    return cmdList;
}


QStringList Power1DspCmdGenerator::getCmdsSumAndAverage()
{
    QStringList dspCmdList;
    // we have to compute sum of our power systems
    dspCmdList.append("ADDVVV(VALPQS,VALPQS+1,VALPQS+3)");
    dspCmdList.append("ADDVVV(VALPQS+2,VALPQS+3,VALPQS+3)");
    // and filter all our values (MeasPhaseCount ???)
    dspCmdList.append(QString("AVERAGE1(4,VALPQS,FILTER)")); // we add results to filter
    return dspCmdList;
}

QStringList Power1DspCmdGenerator::getCmdsFreqOutput(const POWER1MODULE::cPower1ModuleConfigData *configData,
                                                     const QHash<QString, cFoutInfo> foutInfoHash,
                                                     int irqNo)
{
    QStringList dspCmdList;
    if (configData->m_sFreqActualizationMode == "signalperiod") {
        for (int i = 0; i < configData->m_nFreqOutputCount; i++) {
            // which actualvalue do we take as source (offset)
            quint8 actvalueIndex = configData->m_FreqOutputConfList.at(i).m_nSource;
            QString foutSystemName = configData->m_FreqOutputConfList.at(i).m_sName;
            // here we set abs, plus or minus and which frequency output has to be set
            quint16 freqpar = configData->m_FreqOutputConfList.at(i).m_nFoutMode + (foutInfoHash[foutSystemName].dspFoutChannel << 8);
            // frequenzausgang berechnen lassen
            dspCmdList.append(QString("CMPCLK(%1,VALPQS+%2,FREQSCALE+%3)")
                                                    .arg(freqpar)
                                                    .arg(actvalueIndex)
                                                    .arg(i));
        }
    }

    if (configData->m_sIntegrationMode == "time") {
        dspCmdList.append("TESTTIMESKIPNEX(TISTART,TIPAR)");
        dspCmdList.append("ACTIVATECHAIN(1,0x0102)");

        dspCmdList.append("STARTCHAIN(0,1,0x0102)");
        dspCmdList.append("GETSTIME(TISTART)"); // set new system time
        dspCmdList.append(QString("CMPAVERAGE1(4,FILTER,VALPQSF)"));
        dspCmdList.append(QString("CLEARN(%1,FILTER)").arg(2*4+1) );
        dspCmdList.append(QString("DSPINTTRIGGER(0x0,0x%1)").arg(irqNo)); // send interrupt to module

        if (configData->m_sFreqActualizationMode == "integrationtime") {
            for (int i = 0; i < configData->m_nFreqOutputCount; i++) {
                // which actualvalue do we take as source (offset)
                quint8 actvalueIndex = configData->m_FreqOutputConfList.at(i).m_nSource;
                QString foutSystemName =  configData->m_FreqOutputConfList.at(i).m_sName;
                // here we set abs, plus or minus and which frequency output has to be set
                quint16 freqpar = configData->m_FreqOutputConfList.at(i).m_nFoutMode + (foutInfoHash[foutSystemName].dspFoutChannel << 8);
                // frequenzausgang berechnen lassen
                dspCmdList.append(QString("CMPCLK(%1,VALPQSF+%2,FREQSCALE+%3)")
                                                        .arg(freqpar)
                                                        .arg(actvalueIndex)
                                                        .arg(i));
            }
        }
        dspCmdList.append("DEACTIVATECHAIN(1,0x0102)");
        dspCmdList.append("STOPCHAIN(1,0x0102)"); // end processnr., mainchain 1 subchain 2
    }

    else { // otherwise it is period
        dspCmdList.append("TESTVVSKIPLT(N,TIPAR)");
        dspCmdList.append("ACTIVATECHAIN(1,0x0103)");
        dspCmdList.append("STARTCHAIN(0,1,0x0103)");
        dspCmdList.append(QString("CMPAVERAGE1(4,FILTER,VALPQSF)"));
        dspCmdList.append(QString("CLEARN(%1,FILTER)").arg(2*4+1) );
        dspCmdList.append(QString("DSPINTTRIGGER(0x0,0x%1)").arg(irqNo)); // send interrupt to module

        if (configData->m_sFreqActualizationMode == "integrationtime")        {
            for (int i = 0; i < configData->m_nFreqOutputCount; i++) {
                // which actualvalue do we take as source (offset)
                quint8 actvalueIndex = configData->m_FreqOutputConfList.at(i).m_nSource;
                QString foutSystemName =  configData->m_FreqOutputConfList.at(i).m_sName;
                // here we set abs, plus or minus and which frequency output has to be set
                quint16 freqpar = configData->m_FreqOutputConfList.at(i).m_nFoutMode + (foutInfoHash[foutSystemName].dspFoutChannel << 8);
                // frequenzausgang berechnen lassen
                dspCmdList.append(QString("CMPCLK(%1,VALPQSF+%2,FREQSCALE+%3)")
                                                        .arg(freqpar)
                                                        .arg(actvalueIndex)
                                                        .arg(i));
            }
        }
        dspCmdList.append("DEACTIVATECHAIN(1,0x0103)");
        dspCmdList.append("STOPCHAIN(1,0x0103)"); // end processnr., mainchain 1 subchain 2
    }
    return dspCmdList;
}
