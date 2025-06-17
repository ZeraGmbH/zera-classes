#include "power1dspcmdgenerator.h"
#include "dspatomiccommandgen.h"
#include "measmode.h"
#include "measmodeinfo.h"

QStringList Power1DspCmdGenerator::getCmdsInitVars(std::shared_ptr<MeasMode> initialMMode, int samplesPerPeroid, double integrationTime, bool startTiTime, DspChainIdGen &idGen)
{
    QStringList cmdList;
    quint16 chainId = idGen.getNextChainId();
    cmdList.append(DspAtomicCommandGen::getStartChainActive(chainId));
    cmdList.append(QString("CLEARN(%1,MEASSIGNAL1)").arg(samplesPerPeroid) ); // clear meassignal
    cmdList.append(QString("CLEARN(%1,MEASSIGNAL2)").arg(samplesPerPeroid) ); // clear meassignal
    cmdList.append(QString("CLEARN(%1,FILTER)").arg(2*(MeasPhaseCount+SumValueCount)+1) ); // clear the whole filter incl. count
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
    return cmdList;
}

QStringList Power1DspCmdGenerator::getCmdsInitOutputVars()
{
    QStringList cmdList;
    for(int phase=0; phase<MeasPhaseCount; phase++) {
        cmdList.append(QString("SETVAL(VALPQS+%1,0.0)").arg(phase));
        cmdList.append(QString("SETVAL(VAL_APPARENT_P+%1,0.0)").arg(phase));
        cmdList.append(QString("SETVAL(VAL_APPARENT_Q+%1,0.0)").arg(phase));
    }
    return cmdList;
}

QStringList Power1DspCmdGenerator::getCmdsMModeMQREF(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen &idGen)
{
    quint16 chainId = idGen.getNextChainId();
    QStringList cmdList;
    cmdList.append(DspAtomicCommandGen::getActivateChain(chainId));
    cmdList.append(getCmdsSkipOnMModeNotSelected(dspSelectCode, chainId));
    cmdList.append(DspAtomicCommandGen::getStartChainInactive(chainId));

    // we simply set all our actual values to nominal power
    for(int phase=0; phase<measChannelPairList.count(); phase++)
        cmdList.append(QString("COPYVAL(NOMPOWER,VALPQS+%1)").arg(phase));

    cmdList.append(DspAtomicCommandGen::getStopChain(chainId));
    return cmdList;
}

QStringList Power1DspCmdGenerator::getCmdsMMode4LBK(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen &idGen)
{
    quint16 chainId = idGen.getNextChainId();
    QStringList cmdList;
    cmdList.append(DspAtomicCommandGen::getActivateChain(chainId));
    cmdList.append(getCmdsSkipOnMModeNotSelected(dspSelectCode, chainId));
    cmdList.append(DspAtomicCommandGen::getStartChainInactive(chainId));

    // our first measuring system
    cmdList.append(QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL1)")
                          .arg(measChannelPairList[1].voltageChannel)
                          .arg(measChannelPairList[2].voltageChannel));
    cmdList.append("MULCV(MEASSIGNAL1,0.57735027)"); // we correct 1/sqrt(3)
    cmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[0].currentChannel));

    cmdList.append("DFT(1,MEASSIGNAL1,TEMP1)");
    cmdList.append("DFT(1,MEASSIGNAL2,TEMP2)");
    cmdList.append("MULVVV(TEMP1,TEMP2,VALPQS)");
    cmdList.append("MULVVV(TEMP2+1,TEMP1+1,TEMP1)");
    cmdList.append("ADDVVV(TEMP1,VALPQS,VALPQS)");
    cmdList.append("MULVVV(CONST_HALF,VALPQS,VALPQS)");

    //cmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS)");

    // our second measuring system
    cmdList.append(QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL1)")
                          .arg(measChannelPairList[2].voltageChannel)
                          .arg(measChannelPairList[0].voltageChannel));
    cmdList.append("MULCV(MEASSIGNAL1,0.57735027)"); // we correct 1/sqrt(3)
    cmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[1].currentChannel));
    cmdList.append("DFT(1,MEASSIGNAL1,TEMP1)");
    cmdList.append("DFT(1,MEASSIGNAL2,TEMP2)");
    cmdList.append("MULVVV(TEMP1,TEMP2,VALPQS+1)");
    cmdList.append("MULVVV(TEMP2+1,TEMP1+1,TEMP1)");
    cmdList.append("ADDVVV(TEMP1,VALPQS+1,VALPQS+1)");
    cmdList.append("MULVVV(CONST_HALF,VALPQS+1,VALPQS+1)");

    //cmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS+1)");

    // our third measuring system
    cmdList.append(QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL1)")
                          .arg(measChannelPairList[0].voltageChannel)
                          .arg(measChannelPairList[1].voltageChannel));
    cmdList.append("MULCV(MEASSIGNAL1,0.57735027)"); // we correct 1/sqrt(3)
    cmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[2].currentChannel));

    cmdList.append("DFT(1,MEASSIGNAL1,TEMP1)");
    cmdList.append("DFT(1,MEASSIGNAL2,TEMP2)");
    cmdList.append("MULVVV(TEMP1,TEMP2,VALPQS+2)");
    cmdList.append("MULVVV(TEMP2+1,TEMP1+1,TEMP1)");
    cmdList.append("ADDVVV(TEMP1,VALPQS+2,VALPQS+2)");
    cmdList.append("MULVVV(CONST_HALF,VALPQS+2,VALPQS+2)");

    //cmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS+2)");

    cmdList.append(DspAtomicCommandGen::getStopChain(chainId));
    return cmdList;
}


QStringList Power1DspCmdGenerator::getCmdsMMode3LW(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen &idGen)
{
    quint16 chainId = idGen.getNextChainId();
    QStringList cmdList;
    cmdList.append(DspAtomicCommandGen::getActivateChain(chainId));
    cmdList.append(getCmdsSkipOnMModeNotSelected(dspSelectCode, chainId));
    cmdList.append(DspAtomicCommandGen::getStartChainInactive(chainId));

    // our first measuring system
    cmdList.append(QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL1)")
                          .arg(measChannelPairList[0].voltageChannel)
                          .arg(measChannelPairList[1].voltageChannel));
    cmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[0].currentChannel));
    cmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS)");

    // our second measuring system
    cmdList.append("SETVAL(VALPQS+1,0.0)"); // is 0 output

    // our third measuring system
    cmdList.append(QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL1)")
                          .arg(measChannelPairList[2].voltageChannel)
                          .arg(measChannelPairList[1].voltageChannel));
    cmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[2].currentChannel));
    cmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS+2)");

    cmdList.append(DspAtomicCommandGen::getStopChain(chainId));
    return cmdList;
}

QStringList Power1DspCmdGenerator::getCmdsMMode3LB(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen &idGen)
{
    quint16 chainId = idGen.getNextChainId();
    QStringList cmdList;
    cmdList.append(DspAtomicCommandGen::getActivateChain(chainId));
    cmdList.append(getCmdsSkipOnMModeNotSelected(dspSelectCode, chainId));
    cmdList.append(DspAtomicCommandGen::getStartChainInactive(chainId));

    // our first measuring system
    cmdList.append(QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL1)")
                          .arg(measChannelPairList[0].voltageChannel)
                          .arg(measChannelPairList[1].voltageChannel));
    cmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[0].currentChannel));
    cmdList.append("DFT(1,MEASSIGNAL1,TEMP1)");
    cmdList.append("DFT(1,MEASSIGNAL2,TEMP2)");
    cmdList.append("MULVVV(TEMP1,TEMP2+1,VALPQS)");
    cmdList.append("MULVVV(TEMP2,TEMP1+1,TEMP1)");
    cmdList.append("SUBVVV(TEMP1,VALPQS,VALPQS)");
    cmdList.append("MULVVV(CONST_HALF,VALPQS,VALPQS)");

    //cmdList.append("ROTATE(MEASSIGNAL2,270.0)");
    //cmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS)");

    // our second measuring system
    cmdList.append("SETVAL(VALPQS+1,0.0)"); // is 0 output

    // our third measuring system
    cmdList.append(QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL1)")
                          .arg(measChannelPairList[2].voltageChannel)
                          .arg(measChannelPairList[1].voltageChannel));
    cmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[2].currentChannel));
    cmdList.append("DFT(1,MEASSIGNAL1,TEMP1)");
    cmdList.append("DFT(1,MEASSIGNAL2,TEMP2)");
    cmdList.append("MULVVV(TEMP1,TEMP2+1,VALPQS+2)");
    cmdList.append("MULVVV(TEMP2,TEMP1+1,TEMP1)");
    cmdList.append("SUBVVV(TEMP1,VALPQS+2,VALPQS+2)");
    cmdList.append("MULVVV(CONST_HALF,VALPQS+2,VALPQS+2)");

    //cmdList.append("ROTATE(MEASSIGNAL2,270.0)");
    //cmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS+2)");

    cmdList.append(DspAtomicCommandGen::getStopChain(chainId));
    return cmdList;
}

QStringList Power1DspCmdGenerator::getCmdsMMode3LS(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen &idGen)
{
    quint16 chainId = idGen.getNextChainId();
    QStringList cmdList;
    cmdList.append(DspAtomicCommandGen::getActivateChain(chainId));
    cmdList.append(getCmdsSkipOnMModeNotSelected(dspSelectCode, chainId));
    cmdList.append(DspAtomicCommandGen::getStartChainInactive(chainId));

    // S1
    // MEASSIGNAL1 = UL1-UL2
    cmdList.append(QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL1)")
                       .arg(measChannelPairList[0].voltageChannel)
                       .arg(measChannelPairList[1].voltageChannel));
    // TEMP1 = RMS(UL1-UL2)
    cmdList.append("RMS(MEASSIGNAL1,TEMP1)");
    // MEASSIGNAL2 = IL1
    cmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[0].currentChannel));
    // TEMP2 = RMS(IL1)
    cmdList.append("RMS(MEASSIGNAL2,TEMP2)");
    // S1 = RMS(UL1-UL2) * RMS(IL1)
    cmdList.append(QString("MULVVV(TEMP1,TEMP2,VALPQS+0)"));
    // S1 = RMS(UL1-UL2) * RMS(IL1) / sqrt(3)
    cmdList.append("MULVVV(CONST_1_DIV_SQRT3,VALPQS+0,VALPQS+0)");
    // S1 = 1.5* RMS(UL1-UL2) * RMS(IL1) / sqrt(3)
    cmdList.append("MULVVV(CONST_1_5,VALPQS+0,VALPQS+0)");

    // S2 = 0
    cmdList.append("SETVAL(VALPQS+1,0.0)");

    // S3
    // MEASSIGNAL1 = UL3-UL1
    cmdList.append(QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL1)")
                       .arg(measChannelPairList[2].voltageChannel)
                       .arg(measChannelPairList[1].voltageChannel));
    // TEMP1 = RMS(UL3-UL1)
    cmdList.append("RMS(MEASSIGNAL1,TEMP1)");
    // MEASSIGNAL2 = IL3
    cmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[2].currentChannel));
    // TEMP2 = RMS(IL3)
    cmdList.append("RMS(MEASSIGNAL2,TEMP2)");
    // S3 = RMS(UL3-UL1) * RMS(IL3)
    cmdList.append(QString("MULVVV(TEMP1,TEMP2,VALPQS+2)"));
    // S3 = RMS(UL3-UL1) * RMS(IL3) / sqrt(3)
    cmdList.append("MULVVV(CONST_1_DIV_SQRT3,VALPQS+2,VALPQS+2)");
    // S3 = 1.5* RMS(UL1-UL2) * RMS(IL1) / sqrt(3)
    cmdList.append("MULVVV(CONST_1_5,VALPQS+2,VALPQS+2)");

    cmdList.append(DspAtomicCommandGen::getStopChain(chainId));
    return cmdList;
}

QStringList Power1DspCmdGenerator::getCmdsMMode3LSg(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen &idGen)
{
    quint16 chainId = idGen.getNextChainId();
    QStringList cmdList;
    cmdList.append(DspAtomicCommandGen::getActivateChain(chainId));
    cmdList.append(getCmdsSkipOnMModeNotSelected(dspSelectCode, chainId));
    cmdList.append(DspAtomicCommandGen::getStartChainInactive(chainId));

    // S1
    // MEASSIGNAL1 = UL1-UL2
    cmdList.append(QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL1)")
                       .arg(measChannelPairList[0].voltageChannel)
                       .arg(measChannelPairList[1].voltageChannel));
    // MEASSIGNAL2 = IL1
    cmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[0].currentChannel));

    cmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VAL_APPARENT_P+0)"); // P
    cmdList.append("ROTATE(MEASSIGNAL2,270.0)");
    cmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VAL_APPARENT_Q+0)"); // Q
    cmdList.append("ADDVVG(VAL_APPARENT_P+0,VAL_APPARENT_Q+0,VALPQS+0)"); // sqrt(P*P + Q*Q)

    // S2 = 0
    cmdList.append("SETVAL(VALPQS+1,0.0)");

    // S3
    // MEASSIGNAL1 = UL3-UL1
    cmdList.append(QString("COPYDIFF(CH%1,CH%2,MEASSIGNAL1)")
                       .arg(measChannelPairList[2].voltageChannel)
                       .arg(measChannelPairList[1].voltageChannel));
    // MEASSIGNAL2 = IL3
    cmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[2].currentChannel));

    cmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VAL_APPARENT_P+2)"); // P
    cmdList.append("ROTATE(MEASSIGNAL2,270.0)");
    cmdList.append("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VAL_APPARENT_Q+2)"); // Q
    cmdList.append("ADDVVG(VAL_APPARENT_P+2,VAL_APPARENT_Q+2,VALPQS+2)");          // sqrt(P*P + Q*Q)

    cmdList.append(DspAtomicCommandGen::getStopChain(chainId));
    return cmdList;
}

QStringList Power1DspCmdGenerator::getCmdsMModeXLW(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen &idGen)
{
    QStringList cmdList;
    for(int phase=0; phase<measChannelPairList.count(); phase++) {
        quint16 chainId = idGen.getNextChainId();
        cmdList.append(DspAtomicCommandGen::getActivateChain(chainId));
        cmdList.append(getCmdsSkipOnMModeNotSelected(dspSelectCode, chainId));
        cmdList.append(getCmdsSkipOnPhaseNotSelected(phase, chainId));
        cmdList.append(DspAtomicCommandGen::getStartChainInactive(chainId));

        cmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(measChannelPairList[phase].voltageChannel));
        cmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[phase].currentChannel));

        cmdList.append(QString("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS+%1)").arg(phase));

        cmdList.append(DspAtomicCommandGen::getStopChain(chainId));
    }
    return cmdList;
}

QStringList Power1DspCmdGenerator::getCmdsMModeXLB(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen &idGen)
{
    QStringList cmdList;
    for(int phase=0; phase<measChannelPairList.count(); phase++) {
        quint16 chainId = idGen.getNextChainId();
        cmdList.append(DspAtomicCommandGen::getActivateChain(chainId));
        cmdList.append(getCmdsSkipOnMModeNotSelected(dspSelectCode, chainId));
        cmdList.append(getCmdsSkipOnPhaseNotSelected(phase, chainId));
        cmdList.append(DspAtomicCommandGen::getStartChainInactive(chainId));

        cmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(measChannelPairList[phase].voltageChannel));
        cmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[phase].currentChannel));

        cmdList.append("DFT(1,MEASSIGNAL1,TEMP1)");
        cmdList.append("DFT(1,MEASSIGNAL2,TEMP2)");
        cmdList.append(QString("MULVVV(TEMP1,TEMP2+1,VALPQS+%1)").arg(phase));              // VALPQS = Ure*Iim
        cmdList.append(QString("MULVVV(TEMP2,TEMP1+1,TEMP1)"));                             // TEMP1  = Ire*Uim
        cmdList.append(QString("SUBVVV(TEMP1,VALPQS+%1,VALPQS+%2)").arg(phase).arg(phase)); // VALPQS = Ire*Uim - Ure*Iim
        cmdList.append(QString("MULVVV(CONST_HALF,VALPQS+%1,VALPQS+%2)").arg(phase).arg(phase));   // VALPQS = CONST_HALF * (Ire*Uim - Ure*Iim)
        //cmdList.append("ROTATE(MEASSIGNAL2,270.0)");
        //cmdList.append(QString("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VALPQS+%1)").arg(phase));

        cmdList.append(DspAtomicCommandGen::getStopChain(chainId));
    }
    return cmdList;
}

QStringList Power1DspCmdGenerator::getCmdsMModeXLS(int dspSelectCode, MeasSystemChannels measChannelPairList, DspChainIdGen &idGen)
{
    QStringList cmdList;
    for(int phase=0; phase<measChannelPairList.count(); phase++) {
        quint16 chainId = idGen.getNextChainId();
        cmdList.append(DspAtomicCommandGen::getActivateChain(chainId));
        cmdList.append(getCmdsSkipOnMModeNotSelected(dspSelectCode, chainId));
        cmdList.append(getCmdsSkipOnPhaseNotSelected(phase, chainId));
        cmdList.append(DspAtomicCommandGen::getStartChainInactive(chainId));

        cmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(measChannelPairList[phase].voltageChannel));
        cmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[phase].currentChannel));

        cmdList.append("RMS(MEASSIGNAL1,TEMP1)");
        cmdList.append("RMS(MEASSIGNAL2,TEMP2)");
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
        cmdList.append(DspAtomicCommandGen::getActivateChain(chainId));
        cmdList.append(getCmdsSkipOnMModeNotSelected(dspSelectCode, chainId));
        cmdList.append(getCmdsSkipOnPhaseNotSelected(phase, chainId));
        cmdList.append(DspAtomicCommandGen::getStartChainInactive(chainId));

        cmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL1)").arg(measChannelPairList[phase].voltageChannel));
        cmdList.append(QString("COPYDATA(CH%1,0,MEASSIGNAL2)").arg(measChannelPairList[phase].currentChannel));

        cmdList.append(QString("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VAL_APPARENT_P+%1)").arg(phase)); // P
        cmdList.append("ROTATE(MEASSIGNAL2,270.0)");
        cmdList.append(QString("MULCCV(MEASSIGNAL1,MEASSIGNAL2,VAL_APPARENT_Q+%1)").arg(phase)); // Q
        cmdList.append(QString("ADDVVG(VAL_APPARENT_P+%1,VAL_APPARENT_Q+%1,VALPQS+%1)").arg(phase));

        cmdList.append(DspAtomicCommandGen::getStopChain(chainId));
    }
    return cmdList;
}


QStringList Power1DspCmdGenerator::getCmdsSumAndAverage(const DspChainIdGen &idGen)
{
    Q_UNUSED(idGen)
    QStringList cmdList;
    // we have to compute sum of our power systems
    cmdList.append("ADDVVV(VALPQS,VALPQS+1,VALPQS+3)");
    cmdList.append("ADDVVV(VALPQS+2,VALPQS+3,VALPQS+3)");
    // and filter all our values (MeasPhaseCount ???)
    cmdList.append("AVERAGE1(4,VALPQS,FILTER)"); // we add results to filter
    return cmdList;
}

QStringList Power1DspCmdGenerator::getCmdsFreqOutput(const POWER1MODULE::cPower1ModuleConfigData *configData,
                                                     const QMap<QString, cFoutInfo> &foutInfoHash,
                                                     int irqNo,
                                                     DspChainIdGen &idGen)
{
    QStringList cmdList;
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
        cmdList.append("CMPAVERAGE1(4,FILTER,VALPQSF)");
        cmdList.append(QString("CLEARN(%1,FILTER)").arg(2*4+1) );
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
        cmdList.append("TESTVVSKIPLT(N,TIPAR)");
        cmdList.append(QString("ACTIVATECHAIN(1,%1)").arg(dspChainId));
        cmdList.append(QString("STARTCHAIN(0,1,%1)").arg(dspChainId));
        cmdList.append(QString("CMPAVERAGE1(4,FILTER,VALPQSF)"));
        cmdList.append(QString("CLEARN(%1,FILTER)").arg(2*4+1) );
        cmdList.append(QString("DSPINTTRIGGER(0x0,0x%1)").arg(irqNo)); // send interrupt to module

        if (configData->m_sFreqActualizationMode == "integrationtime")        {
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
    return getCmdsSkipOnUnequal("MMODE", dspSelectCode, chainId);
}

QStringList Power1DspCmdGenerator::getCmdsSkipOnPhaseNotSelected(int phase, quint16 chainId)
{
    QStringList cmdList;
    cmdList.append(QString("TESTVCSKIPEQ(XMMODEPHASE%1,1)").arg(phase));
    cmdList.append(DspAtomicCommandGen::getDeactivateChain(chainId));
    return cmdList;
}

