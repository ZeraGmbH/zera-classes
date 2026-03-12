#include "modemodule.h"
#include "modemoduleinit.h"
#include "modemoduleconfigdata.h"
#include "modemoduleconfiguration.h"
#include <proxy.h>
#include <proxyclient.h>
#include <reply.h>
#include <errormessages.h>
#include <pcbinterface.h>
#include <dspinterface.h>

namespace MODEMODULE
{
enum modemoduleinitCmds
{
    setmode,
    writegaincorr,
    writegaincorr2,
    writephasecorr,
    writephasecorr2,
    writeoffsetcorr,
    writeoffsetcorr2,
    subdcdsp,
    setsamplingsystem,
};

cModeModuleInit::cModeModuleInit(cModeModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration) :
    cBaseDspMeasProgram(pConfiguration, module->getVeinModuleName()),
    m_pModule(module),
    m_pcbInterface(std::make_shared<Zera::cPCBInterface>())
{
    m_dspInterface = m_pModule->getServiceInterfaceFactory()->createDspInterfaceMode(m_pModule->getEntityId());

    // m_pcbserverConnectionState.addTransition is done in pcbserverConnection
    m_setModeState.addTransition(this, &cModeModuleInit::activationContinue, &m_dspserverConnectionState);
    // m_dspserverConnectionState.addTransition is done in dspserverConnection
    m_writeGainCorrState.addTransition(this, &cModeModuleInit::activationContinue, &m_writeGainCorr2State);
    m_writeGainCorr2State.addTransition(this, &cModeModuleInit::activationContinue, &m_writePhaseCorrState);
    m_writePhaseCorrState.addTransition(this, &cModeModuleInit::activationContinue, &m_writePhaseCorr2State);
    m_writePhaseCorr2State.addTransition(this, &cModeModuleInit::activationContinue, &m_writeOffsetCorrState);
    m_writeOffsetCorrState.addTransition(this, &cModeModuleInit::activationContinue, &m_writeOffsetCorr2State);
    m_writeOffsetCorr2State.addTransition(this, &cModeModuleInit::activationContinue, &m_setSubDCState);
    m_setSubDCState.addTransition(this, &cModeModuleInit::activationContinue, &m_setSamplingSystemState);
    m_setSamplingSystemState.addTransition(this, &cModeModuleInit::activationContinue, &m_activationDoneState);

    m_activationMachine.addState(&m_pcbserverConnectionState);
    m_activationMachine.addState(&m_setModeState);
    m_activationMachine.addState(&m_dspserverConnectionState);
    m_activationMachine.addState(&m_writeGainCorrState);
    m_activationMachine.addState(&m_writeGainCorr2State);
    m_activationMachine.addState(&m_writePhaseCorrState);
    m_activationMachine.addState(&m_writePhaseCorr2State);
    m_activationMachine.addState(&m_writeOffsetCorrState);
    m_activationMachine.addState(&m_writeOffsetCorr2State);
    m_activationMachine.addState(&m_setSubDCState);
    m_activationMachine.addState(&m_setSamplingSystemState);
    m_activationMachine.addState(&m_activationDoneState);

    m_activationMachine.setInitialState(&m_pcbserverConnectionState);

    connect(&m_pcbserverConnectionState, &QState::entered, this, &cModeModuleInit::pcbserverConnect);
    connect(&m_setModeState, &QState::entered, this, &cModeModuleInit::setMode);
    connect(&m_dspserverConnectionState, &QState::entered, this, &cModeModuleInit::dspserverConnect);
    connect(&m_writeGainCorrState, &QState::entered, this, &cModeModuleInit::writeGainCorr);
    connect(&m_writeGainCorr2State, &QState::entered, this, &cModeModuleInit::writeGainCorr2);
    connect(&m_writePhaseCorrState, &QState::entered, this, &cModeModuleInit::writePhaseCorr);
    connect(&m_writePhaseCorr2State, &QState::entered, this, &cModeModuleInit::writePhaseCorr2);
    connect(&m_writeOffsetCorrState, &QState::entered, this, &cModeModuleInit::writeOffsetCorr);
    connect(&m_writeOffsetCorr2State, &QState::entered, this, &cModeModuleInit::writeOffsetCorr2);
    connect(&m_setSubDCState, &QState::entered, this, &cModeModuleInit::setSubDC);
    connect(&m_setSamplingSystemState, &QState::entered, this, &cModeModuleInit::setSamplingsytem);
    connect(&m_activationDoneState, &QState::entered, this, &cModeModuleInit::activationDone);

    m_deactivationMachine.addState(&m_deactivationDoneState);

    m_deactivationMachine.setInitialState(&m_deactivationDoneState);

    connect(&m_deactivationDoneState, &QState::entered, this, &cModeModuleInit::deactivationDone);

}

void cModeModuleInit::generateVeinInterface()
{
    // at the moment we have no interface
}

cModeModuleConfigData *cModeModuleInit::getConfData()
{
    return qobject_cast<cModeModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();
}

void cModeModuleInit::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    Q_UNUSED(answer)
    if (msgnr == 0) {// 0 was reserved for async. messages
        // that we will ignore
    }
    else {
        if (m_MsgNrCmdList.contains(msgnr)) {
            int cmd = m_MsgNrCmdList.take(msgnr);
            switch (cmd) {
            case setmode:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(setMeasModeErrMsg);
                break;

            case writegaincorr:
            case writegaincorr2:
                if (reply == ack) {
                    emit activationContinue();
                }
                else {
                    // perhaps we emit some error here ?
                    notifyError(writedspgaincorrErrMsg);
                }
                break;

            case writephasecorr:
            case writephasecorr2:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(writedspphasecorrErrMsg);
                break;

            case writeoffsetcorr:
            case writeoffsetcorr2:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(writedspoffsetcorrErrMsg);
                break;

            case subdcdsp:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(writesubdcErrMsg);
                break;

            case setsamplingsystem:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(setsamplingsystemErrmsg);
                break;

            }
        }
    }
}

void cModeModuleInit::pcbserverConnect()
{
    m_pPCBClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_pcbServiceConnectionInfo.m_sIP,
                                                                  m_pModule->getNetworkConfig()->m_pcbServiceConnectionInfo.m_nPort,
                                                                  m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    m_pcbserverConnectionState.addTransition(m_pPCBClient.get(), &Zera::ProxyClient::connected, &m_setModeState);

    m_pcbInterface->setClientSmart(m_pPCBClient);
    connect(m_pcbInterface.get(), &AbstractServerInterface::serverAnswer, this, &cModeModuleInit::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pPCBClient);
}


void cModeModuleInit::setMode()
{
    m_MsgNrCmdList[m_pcbInterface->setMMode(getConfData()->m_sMode)] = setmode;
}


void cModeModuleInit::dspserverConnect()
{
    m_dspClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_dspServiceConnectionInfo,
                                                                 m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    m_dspInterface->setClientSmart(m_dspClient);
    m_dspserverConnectionState.addTransition(m_dspClient.get(), &Zera::ProxyClient::connected, &m_writeGainCorrState);
    connect(m_dspInterface.get(), &AbstractServerInterface::serverAnswer, this, &cModeModuleInit::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_dspClient);
}

void cModeModuleInit::writeGainCorr()
{
    DspVarGroupClientInterface* dspTmpVarGroup = m_dspInterface->createVariableGroup("GainCorrection");
    dspTmpVarGroup->addDspVar("GAINCORRECTION", 32, dspDataTypeFloat, dspInternalSegment);
    float* data = dspTmpVarGroup->data("GAINCORRECTION");
    for (int i = 0; i < 32; i++) {
        data[i] = 1.0;
    }
    m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(dspTmpVarGroup)] = writegaincorr;
}

void cModeModuleInit::writeGainCorr2()
{
    DspVarGroupClientInterface* dspTmpVarGroup = m_dspInterface->createVariableGroup("GainCorrection2");
    dspTmpVarGroup->addDspVar("GAINCORRECTION2", 32, dspDataTypeFloat, dspInternalSegment);
    float* data = dspTmpVarGroup->data("GAINCORRECTION2");
    for (int i = 0; i < 32; i++) {
        data[i] = 1.0;
    }
    m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(dspTmpVarGroup)] = writegaincorr2;
}

void cModeModuleInit::writePhaseCorr()
{
    DspVarGroupClientInterface* dspTmpVarGroup = m_dspInterface->createVariableGroup("PhaseCorrection");
    dspTmpVarGroup->addDspVar("PHASECORRECTION", 32, dspDataTypeFloat, dspInternalSegment);
    float* data = dspTmpVarGroup->data("PHASECORRECTION");
    for (int i = 0; i < 32; i++) {
        data[i] = 0.0;
    }
    m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(dspTmpVarGroup)] = writephasecorr;
}

void cModeModuleInit::writePhaseCorr2()
{
    DspVarGroupClientInterface* dspTmpVarGroup = m_dspInterface->createVariableGroup("PhaseCorrection2");
    dspTmpVarGroup->addDspVar("PHASECORRECTION2", 32, dspDataTypeFloat, dspInternalSegment);
    float* data = dspTmpVarGroup->data("PHASECORRECTION2");
    for (int i = 0; i < 32; i++)
        data[i] = 0.0;
    m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(dspTmpVarGroup)] = writephasecorr2;
}

void cModeModuleInit::writeOffsetCorr()
{
    DspVarGroupClientInterface* dspTmpVarGroup = m_dspInterface->createVariableGroup("OffsetCorrection");
    dspTmpVarGroup->addDspVar("OFFSETCORRECTION", 32, dspDataTypeFloat, dspInternalSegment);
    float* data = dspTmpVarGroup->data("OFFSETCORRECTION");
    for (int i = 0; i < 32; i++)
        data[i] = 0.0;
    m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(dspTmpVarGroup)] = writeoffsetcorr;
}

void cModeModuleInit::writeOffsetCorr2()
{
    DspVarGroupClientInterface* dspTmpVarGroup = m_dspInterface->createVariableGroup("OffsetCorrection2");
    dspTmpVarGroup->addDspVar("OFFSETCORRECTION2", 32, dspDataTypeFloat, dspInternalSegment);
    float* data = dspTmpVarGroup->data("OFFSETCORRECTION2");
    for (int i = 0; i < 32; i++) {
        data[i] = 0.0;
    }
    m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(dspTmpVarGroup)] = writeoffsetcorr;
}

void cModeModuleInit::setSubDC()
{
    DspVarGroupClientInterface* dspTmpVarGroup = m_dspInterface->createVariableGroup("SubDC");
    quint32 subdc = 0;
    // here we can set if sub dc or not
    dspTmpVarGroup->addDspVar("SUBDC", 1, dspDataTypeInt, dspInternalSegment);
    dspTmpVarGroup->setVarData(QString("SUBDC:%1;").arg(subdc));
    m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(dspTmpVarGroup)] = subdcdsp;
}

void cModeModuleInit::setSamplingsytem()
{
    cModeModuleConfigData* confData = getConfData();
    m_MsgNrCmdList[m_dspInterface->setSamplingSystem(confData->m_nChannelnr,
                                                     confData->m_nSignalPeriod,
                                                     confData->m_nMeasurePeriod)] = setsamplingsystem;
}

void cModeModuleInit::activationDone()
{
    emit activated();
}

void cModeModuleInit::deactivationDone()
{
    // and disconnect from our servers afterwards
    disconnect(m_pcbInterface.get(), 0, this, 0);
    emit deactivated();
}

}
