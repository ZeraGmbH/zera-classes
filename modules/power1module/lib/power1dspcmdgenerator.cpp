#include "power1dspcmdgenerator.h"
#include "dspatomiccommandgen.h"
#include "measmode.h"
#include "measmodeinfo.h"
#include <dspinterface.h>

QStringList Power1DspCmdGenerator::getCmdsInitVars(std::shared_ptr<MeasMode> initialMMode, int samplesPerPeroid, double integrationTime, bool startTiTime, DspChainIdGen &idGen)
{
    QStringList cmdList;
    quint16 chainId = idGen.getNextChainId();
    cmdList.append("COMMENT(--- getCmdsInitVars start ---)");
    cmdList.append(DspAtomicCommandGen::getStartChainActive(chainId));
    cmdList.append(QString("CLEARN(%1,FILTER)").arg(DspBuffLen::avgFilterLen(MeasPhaseCount+SumValueCount)));
    cmdList.append(QString("SETVAL(MMODE,%1)").arg(initialMMode->getDspSelectCode()));
    cmdList.append(QString("SETVAL(MMODE_SUM,%1)").arg(initialMMode->getDspSumSelectCode()));

    QString modeMask = initialMMode->getCurrentMask();
    for(int phase=0; phase<modeMask.size(); phase++) {
        QString phaseStr = QString("XMMODEPHASE%1,%3").arg(phase).arg(modeMask.mid(phase,1));
        cmdList.append(QString("SETVAL(%1)").arg(phaseStr)); // initial phases
    }
    cmdList.append("SETVAL(CONST_HALF,0.5)");
    cmdList.append("SETVAL(CONST_1_DIV_SQRT3,0.57735027)");
    cmdList.append("SETVAL(CONST_1_5,1.5)");

    cmdList.append(QString("SETVAL(TIPAR,%1)").arg(integrationTime)); // initial ti time
    if(startTiTime)
        cmdList.append("GETSTIME(TISTART)"); // einmal ti start setzen
    cmdList.append(DspAtomicCommandGen::getDeactivateChain(chainId));
    cmdList.append(DspAtomicCommandGen::getStopChain(chainId));
    cmdList.append("COMMENT(--- getCmdsInitVars end ---)");
    return cmdList;
}

QStringList Power1DspCmdGenerator::getCmdsInitOutputVars()
{
    QStringList cmdList;
    cmdList.append("COMMENT(--- getCmdsInitOutputVars start ---)");
    for(int phase=0; phase<MeasPhaseCount; phase++) {
        cmdList.append(QString("SETVAL(VALPQS+%1,0.0)").arg(phase));
        cmdList.append(QString("SETVAL(VAL_APPARENT_P+%1,0.0)").arg(phase));
        cmdList.append(QString("SETVAL(VAL_APPARENT_Q+%1,0.0)").arg(phase));
    }
    cmdList.append("COMMENT(--- getCmdsInitOutputVars end ---)");
    return cmdList;
}

QStringList Power1DspCmdGenerator::getCmdsMModeMQREF(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen &idGen)
{
    quint16 chainId = idGen.getNextChainId();
    QStringList cmdList;
    cmdList.append("COMMENT(--- getCmdsMModeMQREF start ---)");
    cmdList.append(DspAtomicCommandGen::getActivateChain(chainId));
    cmdList.append(getCmdsSkipOnMModeNotSelected(dspSelectCode, chainId));
    cmdList.append(DspAtomicCommandGen::getStartChainInactive(chainId));

    // we simply set all our actual values to nominal power
    for(int phase=0; phase<measChannelPairList.count(); phase++) {
        cmdList.append(QString("COMMENT(--- Phase %1 ---)").arg(phase));
        cmdList.append(QString("COPYVAL(NOMPOWER,VALPQS+%1)").arg(phase));
    }

    cmdList.append(DspAtomicCommandGen::getStopChain(chainId));
    cmdList.append("COMMENT(--- getCmdsMModeMQREF end ---)");
    return cmdList;
}

QStringList Power1DspCmdGenerator::getCmdsMMode4LBK(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen &idGen)
{
    quint16 chainId = idGen.getNextChainId();
    QStringList cmdList;
    cmdList.append("COMMENT(--- getCmdsMMode4LBK start ---)");
    cmdList.append(DspAtomicCommandGen::getActivateChain(chainId));
    cmdList.append(getCmdsSkipOnMModeNotSelected(dspSelectCode, chainId));
    cmdList.append(DspAtomicCommandGen::getStartChainInactive(chainId));

    // our first measuring system
    cmdList.append(QString("COPYDIFF(CH%1,CH%2,TMP_SAMPLES_SINGLE_1)")
                          .arg(measChannelPairList[1].voltageChannel)
                          .arg(measChannelPairList[2].voltageChannel));
    cmdList.append("MULCV(TMP_SAMPLES_SINGLE_1,0.57735027)"); // we correct 1/√3
    cmdList.append(QString("COPYDATA(CH%1,0,TMP_SAMPLES_SINGLE_2)").arg(measChannelPairList[0].currentChannel));

    cmdList.append("DFT(1,TMP_SAMPLES_SINGLE_1,TEMP1)");
    cmdList.append("DFT(1,TMP_SAMPLES_SINGLE_2,TEMP2)");
    cmdList.append("MULVVV(TEMP1,TEMP2,VALPQS)");
    cmdList.append("MULVVV(TEMP2+1,TEMP1+1,TEMP1)");
    cmdList.append("ADDVVV(TEMP1,VALPQS,VALPQS)");
    cmdList.append("MULVVV(CONST_HALF,VALPQS,VALPQS)");

    //cmdList.append("MULCCV(TMP_SAMPLES_SINGLE_1,TMP_SAMPLES_SINGLE_2,VALPQS)");

    // our second measuring system
    cmdList.append(QString("COPYDIFF(CH%1,CH%2,TMP_SAMPLES_SINGLE_1)")
                          .arg(measChannelPairList[2].voltageChannel)
                          .arg(measChannelPairList[0].voltageChannel));
    cmdList.append("MULCV(TMP_SAMPLES_SINGLE_1,0.57735027)"); // we correct 1/√3
    cmdList.append(QString("COPYDATA(CH%1,0,TMP_SAMPLES_SINGLE_2)").arg(measChannelPairList[1].currentChannel));
    cmdList.append("DFT(1,TMP_SAMPLES_SINGLE_1,TEMP1)");
    cmdList.append("DFT(1,TMP_SAMPLES_SINGLE_2,TEMP2)");
    cmdList.append("MULVVV(TEMP1,TEMP2,VALPQS+1)");
    cmdList.append("MULVVV(TEMP2+1,TEMP1+1,TEMP1)");
    cmdList.append("ADDVVV(TEMP1,VALPQS+1,VALPQS+1)");
    cmdList.append("MULVVV(CONST_HALF,VALPQS+1,VALPQS+1)");

    //cmdList.append("MULCCV(TMP_SAMPLES_SINGLE_1,TMP_SAMPLES_SINGLE_2,VALPQS+1)");

    // our third measuring system
    cmdList.append(QString("COPYDIFF(CH%1,CH%2,TMP_SAMPLES_SINGLE_1)")
                          .arg(measChannelPairList[0].voltageChannel)
                          .arg(measChannelPairList[1].voltageChannel));
    cmdList.append("MULCV(TMP_SAMPLES_SINGLE_1,0.57735027)"); // we correct 1/√3
    cmdList.append(QString("COPYDATA(CH%1,0,TMP_SAMPLES_SINGLE_2)").arg(measChannelPairList[2].currentChannel));

    cmdList.append("DFT(1,TMP_SAMPLES_SINGLE_1,TEMP1)");
    cmdList.append("DFT(1,TMP_SAMPLES_SINGLE_2,TEMP2)");
    cmdList.append("MULVVV(TEMP1,TEMP2,VALPQS+2)");
    cmdList.append("MULVVV(TEMP2+1,TEMP1+1,TEMP1)");
    cmdList.append("ADDVVV(TEMP1,VALPQS+2,VALPQS+2)");
    cmdList.append("MULVVV(CONST_HALF,VALPQS+2,VALPQS+2)");

    //cmdList.append("MULCCV(TMP_SAMPLES_SINGLE_1,TMP_SAMPLES_SINGLE_2,VALPQS+2)");

    cmdList.append(DspAtomicCommandGen::getStopChain(chainId));
    cmdList.append("COMMENT(--- getCmdsMMode4LBK end ---)");
    return cmdList;
}


QStringList Power1DspCmdGenerator::getCmdsMMode3LW(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen &idGen)
{
    quint16 chainId = idGen.getNextChainId();
    QStringList cmdList;
    cmdList.append("COMMENT(--- getCmdsMMode3LW start ---)");
    cmdList.append(DspAtomicCommandGen::getActivateChain(chainId));
    cmdList.append(getCmdsSkipOnMModeNotSelected(dspSelectCode, chainId));
    cmdList.append(DspAtomicCommandGen::getStartChainInactive(chainId));

    // our first measuring system
    cmdList.append(QString("COPYDIFF(CH%1,CH%2,TMP_SAMPLES_SINGLE_1)")
                          .arg(measChannelPairList[0].voltageChannel)
                          .arg(measChannelPairList[1].voltageChannel));
    cmdList.append(QString("COPYDATA(CH%1,0,TMP_SAMPLES_SINGLE_2)").arg(measChannelPairList[0].currentChannel));
    cmdList.append("MULCCV(TMP_SAMPLES_SINGLE_1,TMP_SAMPLES_SINGLE_2,VALPQS)");

    // our second measuring system
    cmdList.append("SETVAL(VALPQS+1,0.0)"); // is 0 output

    // our third measuring system
    cmdList.append(QString("COPYDIFF(CH%1,CH%2,TMP_SAMPLES_SINGLE_1)")
                          .arg(measChannelPairList[2].voltageChannel)
                          .arg(measChannelPairList[1].voltageChannel));
    cmdList.append(QString("COPYDATA(CH%1,0,TMP_SAMPLES_SINGLE_2)").arg(measChannelPairList[2].currentChannel));
    cmdList.append("MULCCV(TMP_SAMPLES_SINGLE_1,TMP_SAMPLES_SINGLE_2,VALPQS+2)");

    cmdList.append(DspAtomicCommandGen::getStopChain(chainId));
    cmdList.append("COMMENT(--- getCmdsMMode3LW end ---)");
    return cmdList;
}

QStringList Power1DspCmdGenerator::getCmdsMMode3LB(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen &idGen)
{
    quint16 chainId = idGen.getNextChainId();
    QStringList cmdList;
    cmdList.append("COMMENT(--- getCmdsMMode3LB start ---)");
    cmdList.append(DspAtomicCommandGen::getActivateChain(chainId));
    cmdList.append(getCmdsSkipOnMModeNotSelected(dspSelectCode, chainId));
    cmdList.append(DspAtomicCommandGen::getStartChainInactive(chainId));

    // our first measuring system
    cmdList.append(QString("COPYDIFF(CH%1,CH%2,TMP_SAMPLES_SINGLE_1)")
                          .arg(measChannelPairList[0].voltageChannel)
                          .arg(measChannelPairList[1].voltageChannel));
    cmdList.append(QString("COPYDATA(CH%1,0,TMP_SAMPLES_SINGLE_2)").arg(measChannelPairList[0].currentChannel));
    cmdList.append("DFT(1,TMP_SAMPLES_SINGLE_1,TEMP1)");
    cmdList.append("DFT(1,TMP_SAMPLES_SINGLE_2,TEMP2)");
    cmdList.append("MULVVV(TEMP1,TEMP2+1,VALPQS)");
    cmdList.append("MULVVV(TEMP2,TEMP1+1,TEMP1)");
    cmdList.append("SUBVVV(TEMP1,VALPQS,VALPQS)");
    cmdList.append("MULVVV(CONST_HALF,VALPQS,VALPQS)");

    //cmdList.append("ROTATE(TMP_SAMPLES_SINGLE_2,270.0)");
    //cmdList.append("MULCCV(TMP_SAMPLES_SINGLE_1,TMP_SAMPLES_SINGLE_2,VALPQS)");

    // our second measuring system
    cmdList.append("SETVAL(VALPQS+1,0.0)"); // is 0 output

    // our third measuring system
    cmdList.append(QString("COPYDIFF(CH%1,CH%2,TMP_SAMPLES_SINGLE_1)")
                          .arg(measChannelPairList[2].voltageChannel)
                          .arg(measChannelPairList[1].voltageChannel));
    cmdList.append(QString("COPYDATA(CH%1,0,TMP_SAMPLES_SINGLE_2)").arg(measChannelPairList[2].currentChannel));
    cmdList.append("DFT(1,TMP_SAMPLES_SINGLE_1,TEMP1)");
    cmdList.append("DFT(1,TMP_SAMPLES_SINGLE_2,TEMP2)");
    cmdList.append("MULVVV(TEMP1,TEMP2+1,VALPQS+2)");
    cmdList.append("MULVVV(TEMP2,TEMP1+1,TEMP1)");
    cmdList.append("SUBVVV(TEMP1,VALPQS+2,VALPQS+2)");
    cmdList.append("MULVVV(CONST_HALF,VALPQS+2,VALPQS+2)");

    //cmdList.append("ROTATE(TMP_SAMPLES_SINGLE_2,270.0)");
    //cmdList.append("MULCCV(TMP_SAMPLES_SINGLE_1,TMP_SAMPLES_SINGLE_2,VALPQS+2)");

    cmdList.append(DspAtomicCommandGen::getStopChain(chainId));
    cmdList.append("COMMENT(--- getCmdsMMode3LB end ---)");
    return cmdList;
}

QStringList Power1DspCmdGenerator::getCmdsMMode3LS(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen &idGen)
{
    quint16 chainId = idGen.getNextChainId();
    QStringList cmdList;
    cmdList.append("COMMENT(--- getCmdsMMode3LS start ---)");
    cmdList.append(DspAtomicCommandGen::getActivateChain(chainId));
    cmdList.append(getCmdsSkipOnMModeNotSelected(dspSelectCode, chainId));
    cmdList.append(DspAtomicCommandGen::getStartChainInactive(chainId));

    // S1
    // TMP_SAMPLES_SINGLE_1 = UL1-UL2
    cmdList.append(QString("COPYDIFF(CH%1,CH%2,TMP_SAMPLES_SINGLE_1)")
                       .arg(measChannelPairList[0].voltageChannel)
                       .arg(measChannelPairList[1].voltageChannel));
    // TEMP1 = RMS(UL1-UL2)
    cmdList.append("RMS(TMP_SAMPLES_SINGLE_1,TEMP1)");
    // TMP_SAMPLES_SINGLE_2 = IL1
    cmdList.append(QString("COPYDATA(CH%1,0,TMP_SAMPLES_SINGLE_2)").arg(measChannelPairList[0].currentChannel));
    // TEMP2 = RMS(IL1)
    cmdList.append("RMS(TMP_SAMPLES_SINGLE_2,TEMP2)");
    // S1 = RMS(UL1-UL2) * RMS(IL1)
    cmdList.append(QString("MULVVV(TEMP1,TEMP2,VALPQS+0)"));
    // S1 = RMS(UL1-UL2) * RMS(IL1) / √3
    cmdList.append("MULVVV(CONST_1_DIV_SQRT3,VALPQS+0,VALPQS+0)");
    // S1 = 1.5* RMS(UL1-UL2) * RMS(IL1) / √3
    cmdList.append("MULVVV(CONST_1_5,VALPQS+0,VALPQS+0)");

    // S2 = 0
    cmdList.append("SETVAL(VALPQS+1,0.0)");

    // S3
    // TMP_SAMPLES_SINGLE_1 = UL3-UL1
    cmdList.append(QString("COPYDIFF(CH%1,CH%2,TMP_SAMPLES_SINGLE_1)")
                       .arg(measChannelPairList[2].voltageChannel)
                       .arg(measChannelPairList[1].voltageChannel));
    // TEMP1 = RMS(UL3-UL1)
    cmdList.append("RMS(TMP_SAMPLES_SINGLE_1,TEMP1)");
    // TMP_SAMPLES_SINGLE_2 = IL3
    cmdList.append(QString("COPYDATA(CH%1,0,TMP_SAMPLES_SINGLE_2)").arg(measChannelPairList[2].currentChannel));
    // TEMP2 = RMS(IL3)
    cmdList.append("RMS(TMP_SAMPLES_SINGLE_2,TEMP2)");
    // S3 = RMS(UL3-UL1) * RMS(IL3)
    cmdList.append(QString("MULVVV(TEMP1,TEMP2,VALPQS+2)"));
    // S3 = RMS(UL3-UL1) * RMS(IL3) / √3
    cmdList.append("MULVVV(CONST_1_DIV_SQRT3,VALPQS+2,VALPQS+2)");
    // S3 = 1.5* RMS(UL1-UL2) * RMS(IL1) / √3
    cmdList.append("MULVVV(CONST_1_5,VALPQS+2,VALPQS+2)");

    cmdList.append(DspAtomicCommandGen::getStopChain(chainId));
    cmdList.append("COMMENT(--- getCmdsMMode3LS end ---)");
    return cmdList;
}

QStringList Power1DspCmdGenerator::getCmdsMMode3LSg(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen &idGen)
{
    quint16 chainId = idGen.getNextChainId();
    QStringList cmdList;
    cmdList.append("COMMENT(--- getCmdsMMode3LSg start ---)");
    cmdList.append(DspAtomicCommandGen::getActivateChain(chainId));
    cmdList.append(getCmdsSkipOnMModeNotSelected(dspSelectCode, chainId));
    cmdList.append(DspAtomicCommandGen::getStartChainInactive(chainId));

    // S1
    // TMP_SAMPLES_SINGLE_1 = UL1-UL2
    cmdList.append(QString("COPYDIFF(CH%1,CH%2,TMP_SAMPLES_SINGLE_1)")
                       .arg(measChannelPairList[0].voltageChannel)
                       .arg(measChannelPairList[1].voltageChannel));
    // TMP_SAMPLES_SINGLE_2 = IL1
    cmdList.append(QString("COPYDATA(CH%1,0,TMP_SAMPLES_SINGLE_2)").arg(measChannelPairList[0].currentChannel));

    cmdList.append("MULCCV(TMP_SAMPLES_SINGLE_1,TMP_SAMPLES_SINGLE_2,VAL_APPARENT_P+0)");     // P
    cmdList.append("ROTATE(TMP_SAMPLES_SINGLE_2,270.0)");
    cmdList.append("MULCCV(TMP_SAMPLES_SINGLE_1,TMP_SAMPLES_SINGLE_2,VAL_APPARENT_Q+0)");     // Q
    cmdList.append("ADDVVG(VAL_APPARENT_P+0,VAL_APPARENT_Q+0,VALPQS+0)");   // √(P² + Q²)

    // S2 = 0
    cmdList.append("SETVAL(VALPQS+1,0.0)");

    // S3
    // TMP_SAMPLES_SINGLE_1 = UL3-UL1
    cmdList.append(QString("COPYDIFF(CH%1,CH%2,TMP_SAMPLES_SINGLE_1)")
                       .arg(measChannelPairList[2].voltageChannel)
                       .arg(measChannelPairList[1].voltageChannel));
    // TMP_SAMPLES_SINGLE_2 = IL3
    cmdList.append(QString("COPYDATA(CH%1,0,TMP_SAMPLES_SINGLE_2)").arg(measChannelPairList[2].currentChannel));

    cmdList.append("MULCCV(TMP_SAMPLES_SINGLE_1,TMP_SAMPLES_SINGLE_2,VAL_APPARENT_P+2)");     // P
    cmdList.append("ROTATE(TMP_SAMPLES_SINGLE_2,270.0)");
    cmdList.append("MULCCV(TMP_SAMPLES_SINGLE_1,TMP_SAMPLES_SINGLE_2,VAL_APPARENT_Q+2)");     // Q
    cmdList.append("ADDVVG(VAL_APPARENT_P+2,VAL_APPARENT_Q+2,VALPQS+2)");   // √(P² + Q²)

    cmdList.append(DspAtomicCommandGen::getStopChain(chainId));
    cmdList.append("COMMENT(--- getCmdsMMode3LSg end ---)");
    return cmdList;
}

QStringList Power1DspCmdGenerator::getCmdsMModeXLW(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen &idGen)
{
    QStringList cmdList;
    for(int phase=0; phase<measChannelPairList.count(); phase++) {
        quint16 chainId = idGen.getNextChainId();
        cmdList.append(QString("COMMENT(--- getCmdsMModeXLW phase %1 ---)").arg(phase));
        cmdList.append(DspAtomicCommandGen::getActivateChain(chainId));
        cmdList.append(getCmdsSkipOnMModeNotSelected(dspSelectCode, chainId));
        cmdList.append(getCmdsSkipOnPhaseNotSelected(phase, chainId));
        cmdList.append(DspAtomicCommandGen::getStartChainInactive(chainId));

        cmdList.append(QString("COPYDATA(CH%1,0,TMP_SAMPLES_SINGLE_1)").arg(measChannelPairList[phase].voltageChannel));
        cmdList.append(QString("COPYDATA(CH%1,0,TMP_SAMPLES_SINGLE_2)").arg(measChannelPairList[phase].currentChannel));

        cmdList.append(QString("MULCCV(TMP_SAMPLES_SINGLE_1,TMP_SAMPLES_SINGLE_2,VALPQS+%1)").arg(phase));

        cmdList.append(DspAtomicCommandGen::getStopChain(chainId));
    }
    return cmdList;
}

QStringList Power1DspCmdGenerator::getCmdsMModeXLB(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen &idGen)
{
    QStringList cmdList;
    for(int phase=0; phase<measChannelPairList.count(); phase++) {
        quint16 chainId = idGen.getNextChainId();
        cmdList.append(QString("COMMENT(--- getCmdsMModeXLB phase %1 ---)").arg(phase));
        cmdList.append(DspAtomicCommandGen::getActivateChain(chainId));
        cmdList.append(getCmdsSkipOnMModeNotSelected(dspSelectCode, chainId));
        cmdList.append(getCmdsSkipOnPhaseNotSelected(phase, chainId));
        cmdList.append(DspAtomicCommandGen::getStartChainInactive(chainId));

        cmdList.append(QString("COPYDATA(CH%1,0,TMP_SAMPLES_SINGLE_1)").arg(measChannelPairList[phase].voltageChannel));
        cmdList.append(QString("COPYDATA(CH%1,0,TMP_SAMPLES_SINGLE_2)").arg(measChannelPairList[phase].currentChannel));

        cmdList.append("DFT(1,TMP_SAMPLES_SINGLE_1,TEMP1)");
        cmdList.append("DFT(1,TMP_SAMPLES_SINGLE_2,TEMP2)");
        cmdList.append(QString("MULVVV(TEMP1,TEMP2+1,VALPQS+%1)").arg(phase));          // VALPQS = Ure*Iim
        cmdList.append(QString("MULVVV(TEMP2,TEMP1+1,TEMP1)"));                         // TEMP1  = Ire*Uim
        cmdList.append(QString("SUBVVV(TEMP1,VALPQS+%1,VALPQS+%1)").arg(phase));        // VALPQS = Ire*Uim - Ure*Iim
        cmdList.append(QString("MULVVV(CONST_HALF,VALPQS+%1,VALPQS+%1)").arg(phase));   // VALPQS = 0.5 * (Ire*Uim - Ure*Iim)
        //cmdList.append("ROTATE(TMP_SAMPLES_SINGLE_2,270.0)");
        //cmdList.append(QString("MULCCV(TMP_SAMPLES_SINGLE_1,TMP_SAMPLES_SINGLE_2,VALPQS+%1)").arg(phase));

        cmdList.append(DspAtomicCommandGen::getStopChain(chainId));
    }
    return cmdList;
}

QStringList Power1DspCmdGenerator::getCmdsMModeXLS(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen &idGen)
{
    QStringList cmdList;
    for(int phase=0; phase<measChannelPairList.count(); phase++) {
        quint16 chainId = idGen.getNextChainId();
        cmdList.append(QString("COMMENT(--- getCmdsMModeXLS phase %1 ---)").arg(phase));
        cmdList.append(DspAtomicCommandGen::getActivateChain(chainId));
        cmdList.append(getCmdsSkipOnMModeNotSelected(dspSelectCode, chainId));
        cmdList.append(getCmdsSkipOnPhaseNotSelected(phase, chainId));
        cmdList.append(DspAtomicCommandGen::getStartChainInactive(chainId));

        cmdList.append(QString("COPYDATA(CH%1,0,TMP_SAMPLES_SINGLE_1)").arg(measChannelPairList[phase].voltageChannel));
        cmdList.append(QString("COPYDATA(CH%1,0,TMP_SAMPLES_SINGLE_2)").arg(measChannelPairList[phase].currentChannel));

        cmdList.append("RMS(TMP_SAMPLES_SINGLE_1,TEMP1)");
        cmdList.append("RMS(TMP_SAMPLES_SINGLE_2,TEMP2)");
        cmdList.append(QString("MULVVV(TEMP1,TEMP2,VALPQS+%1)").arg(phase));

        cmdList.append(DspAtomicCommandGen::getStopChain(chainId));
    }
    return cmdList;
}

QStringList Power1DspCmdGenerator::getCmdsMModeXLSg(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen &idGen)
{
    QStringList cmdList;
    for(int phase=0; phase<measChannelPairList.count(); phase++) {
        quint16 chainId = idGen.getNextChainId();
        cmdList.append(QString("COMMENT(--- getCmdsMModeXLSg phase %1 ---)").arg(phase));
        cmdList.append(DspAtomicCommandGen::getActivateChain(chainId));
        cmdList.append(getCmdsSkipOnMModeNotSelected(dspSelectCode, chainId));
        cmdList.append(getCmdsSkipOnPhaseNotSelected(phase, chainId));
        cmdList.append(DspAtomicCommandGen::getStartChainInactive(chainId));

        cmdList.append(QString("COPYDATA(CH%1,0,TMP_SAMPLES_SINGLE_1)").arg(measChannelPairList[phase].voltageChannel));
        cmdList.append(QString("COPYDATA(CH%1,0,TMP_SAMPLES_SINGLE_2)").arg(measChannelPairList[phase].currentChannel));

        cmdList.append(QString("MULCCV(TMP_SAMPLES_SINGLE_1,TMP_SAMPLES_SINGLE_2,VAL_APPARENT_P+%1)").arg(phase)); // P
        cmdList.append("ROTATE(TMP_SAMPLES_SINGLE_2,270.0)");
        cmdList.append(QString("MULCCV(TMP_SAMPLES_SINGLE_1,TMP_SAMPLES_SINGLE_2,VAL_APPARENT_Q+%1)").arg(phase)); // Q
        cmdList.append(QString("ADDVVG(VAL_APPARENT_P+%1,VAL_APPARENT_Q+%1,VALPQS+%1)").arg(phase));

        cmdList.append(DspAtomicCommandGen::getStopChain(chainId));
    }
    return cmdList;
}


QStringList Power1DspCmdGenerator::getCmdsSumAndAverage(DspChainIdGen &idGen)
{
    QStringList cmdList;

    cmdList.append("COMMENT(--- getCmdsSumAndAverage: If MMODE_SUM == sum_phasevals ---)");
    quint16 chainIdPhaseSum = idGen.getNextChainId();
    cmdList.append(DspAtomicCommandGen::getActivateChain(chainIdPhaseSum));
    cmdList.append(getCmdsSkipOnUnequal("MMODE_SUM", sum_phasevals, chainIdPhaseSum));
    cmdList.append(DspAtomicCommandGen::getStartChainInactive(chainIdPhaseSum));
    cmdList.append("ADDVVV(VALPQS+0,VALPQS+1,VALPQS+3)");
    cmdList.append("ADDVVV(VALPQS+2,VALPQS+3,VALPQS+3)");
    cmdList.append(DspAtomicCommandGen::getStopChain(chainIdPhaseSum));

    cmdList.append("COMMENT(--- getCmdsSumAndAverage: If MMODE_SUM == sum_phase_pq_geom ---)");
    quint16 chainIdPhasePQGeom = idGen.getNextChainId();
    cmdList.append(DspAtomicCommandGen::getActivateChain(chainIdPhasePQGeom));
    cmdList.append(getCmdsSkipOnUnequal("MMODE_SUM", sum_phase_pq_geom, chainIdPhasePQGeom));
    cmdList.append(DspAtomicCommandGen::getStartChainInactive(chainIdPhasePQGeom));
    // TEMP1 = ∑P
    cmdList.append("ADDVVV(VAL_APPARENT_P+0,VAL_APPARENT_P+1,TEMP1)");
    cmdList.append("ADDVVV(VAL_APPARENT_P+2,TEMP1,TEMP1)");
    // TEMP2 = ∑Q
    cmdList.append("ADDVVV(VAL_APPARENT_Q+0,VAL_APPARENT_Q+1,TEMP2)");
    cmdList.append("ADDVVV(VAL_APPARENT_Q+2,TEMP2,TEMP2)");
     // VALPQS+3 = √((∑P)² + (∑Q)²)
    cmdList.append("ADDVVG(TEMP1,TEMP2,VALPQS+3)");
    cmdList.append(DspAtomicCommandGen::getStopChain(chainIdPhasePQGeom));

    // and filter all our values
    cmdList.append(QString("AVERAGE1(%1,VALPQS,FILTER)").arg(MeasPhaseCount+SumValueCount)); // we add results to filter
    cmdList.append("COMMENT(--- getCmdsSumAndAverage end ---)");

    return cmdList;
}

QStringList Power1DspCmdGenerator::getCmdsFreqOutput(const POWER1MODULE::cPower1ModuleConfigData *configData,
                                                     const QMap<QString, cFoutInfo> &foutInfoHash,
                                                     int irqNo,
                                                     DspChainIdGen &idGen)
{
    QStringList cmdList;
    cmdList.append("COMMENT(--- getCmdsFreqOutput start ---)");
    if (configData->m_sFreqActualizationMode == "signalperiod") {
        for (int i = 0; i < configData->m_nFreqOutputCount; i++) {
            // which actualvalue do we take as source (offset)
            quint8 actvalueIndex = configData->m_FreqOutputConfList.at(i).m_nSource;
            QString foutSystemName = configData->m_FreqOutputConfList.at(i).m_sName;
            // here we set abs, plus or minus and which frequency output has to be set
            quint16 freqpar = configData->m_FreqOutputConfList.at(i).m_nFoutMode + (foutInfoHash[foutSystemName].dspFoutChannel << 8);
            // frequenzausgang berechnen lassen
            cmdList.append(QString("CMPCLK(%1,VALPQS+%2,FREQSCALE+%3)")
                                                    .arg(freqpar)
                                                    .arg(actvalueIndex)
                                                    .arg(i));
        }
    }

    if (configData->m_sIntegrationMode == "time") {
        QString dspChainId = idGen.getNextChainIdStr();
        cmdList.append("TESTTIMESKIPNEX(TISTART,TIPAR)");
        cmdList.append(QString("ACTIVATECHAIN(1,%1)").arg(dspChainId));

        cmdList.append(QString("STARTCHAIN(0,1,%1)").arg(dspChainId));
        cmdList.append("GETSTIME(TISTART)"); // set new system time
        cmdList.append(QString("CMPAVERAGE1(%1,FILTER,VALPQSF)").arg(MeasPhaseCount+SumValueCount));
        cmdList.append(QString("CLEARN(%1,FILTER)").arg(DspBuffLen::avgFilterLen(MeasPhaseCount+SumValueCount)));
        cmdList.append(QString("DSPINTTRIGGER(0x0,0x%1)").arg(/* dummy */ 0)); // send interrupt to module

        if (configData->m_sFreqActualizationMode == "integrationtime") {
            for (int i = 0; i < configData->m_nFreqOutputCount; i++) {
                // which actualvalue do we take as source (offset)
                quint8 actvalueIndex = configData->m_FreqOutputConfList.at(i).m_nSource;
                QString foutSystemName =  configData->m_FreqOutputConfList.at(i).m_sName;
                // here we set abs, plus or minus and which frequency output has to be set
                quint16 freqpar = configData->m_FreqOutputConfList.at(i).m_nFoutMode + (foutInfoHash[foutSystemName].dspFoutChannel << 8);
                // frequenzausgang berechnen lassen
                cmdList.append(QString("CMPCLK(%1,VALPQSF+%2,FREQSCALE+%3)")
                                                        .arg(freqpar)
                                                        .arg(actvalueIndex)
                                                        .arg(i));
            }
        }
        cmdList.append(QString("DEACTIVATECHAIN(1,%1)").arg(dspChainId));
        cmdList.append(QString("STOPCHAIN(1,%1)").arg(dspChainId)); // end processnr., mainchain 1 subchain 2
    }

    else { // otherwise it is period
        QString dspChainId = idGen.getNextChainIdStr();
        cmdList.append(QString("TESTVVSKIPLT(FILTER+%1,TIPAR)").arg(DspBuffLen::avgFilterCountPos(MeasPhaseCount+SumValueCount)));
        cmdList.append(QString("ACTIVATECHAIN(1,%1)").arg(dspChainId));
        cmdList.append(QString("STARTCHAIN(0,1,%1)").arg(dspChainId));
        cmdList.append(QString("CMPAVERAGE1(%1,FILTER,VALPQSF)").arg(MeasPhaseCount+SumValueCount));
        cmdList.append(QString("CLEARN(%1,FILTER)").arg(DspBuffLen::avgFilterLen(MeasPhaseCount+SumValueCount)));
        cmdList.append(QString("DSPINTTRIGGER(0x0,0x%1)").arg(irqNo)); // send interrupt to module

        if (configData->m_sFreqActualizationMode == "integrationtime") {
            for (int i = 0; i < configData->m_nFreqOutputCount; i++) {
                // which actualvalue do we take as source (offset)
                quint8 actvalueIndex = configData->m_FreqOutputConfList.at(i).m_nSource;
                QString foutSystemName =  configData->m_FreqOutputConfList.at(i).m_sName;
                // here we set abs, plus or minus and which frequency output has to be set
                quint16 freqpar = configData->m_FreqOutputConfList.at(i).m_nFoutMode + (foutInfoHash[foutSystemName].dspFoutChannel << 8);
                // frequenzausgang berechnen lassen
                cmdList.append(QString("CMPCLK(%1,VALPQSF+%2,FREQSCALE+%3)")
                                                        .arg(freqpar)
                                                        .arg(actvalueIndex)
                                                        .arg(i));
            }
        }
        cmdList.append(QString("DEACTIVATECHAIN(1,%1)").arg(dspChainId));
        cmdList.append(QString("STOPCHAIN(1,%1)").arg(dspChainId)); // end processnr., mainchain 1 subchain 2
    }
    cmdList.append("COMMENT(--- getCmdsFreqOutput end ---)");
    return cmdList;
}

QStringList Power1DspCmdGenerator::getCmdsSkipOnUnequal(const QString &varName, int varValue, quint16 chainId)
{
    QStringList cmdList;
    cmdList.append(QString("TESTVCSKIPEQ(%1,%2)").arg(varName).arg(varValue));
    cmdList.append(DspAtomicCommandGen::getDeactivateChain(chainId));
    return cmdList;
}

QStringList Power1DspCmdGenerator::getCmdsSkipOnMModeNotSelected(int dspSelectCode, quint16 chainId)
{
    QStringList cmdList;
    cmdList.append(getCmdsSkipOnUnequal("MMODE", dspSelectCode, chainId));
    return cmdList;
}

QStringList Power1DspCmdGenerator::getCmdsSkipOnPhaseNotSelected(int phase, quint16 chainId)
{
    QStringList cmdList;
    cmdList.append(QString("TESTVCSKIPEQ(XMMODEPHASE%1,1)").arg(phase));
    cmdList.append(DspAtomicCommandGen::getDeactivateChain(chainId));
    return cmdList;
}

