#include "modemodule.h"
#include "modemoduleinit.h"
#include "modemoduleconfigdata.h"
#include <proxy.h>
#include <proxyclient.h>
#include <reply.h>
#include <errormessages.h>
#include <rminterface.h>
#include <pcbinterface.h>
#include <dspinterface.h>

namespace MODEMODULE
{

cModeModuleInit::cModeModuleInit(cModeModule* module, cModeModuleConfigData& configData) :
    cModuleActivist(module->getVeinModuleName()),
    m_pModule(module),
    m_ConfigData(configData)
{
    m_pcbInterface = std::make_shared<Zera::cPCBInterface>();
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


void cModeModuleInit::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if (msgnr == 0) {// 0 was reserved for async. messages
        // that we will ignore
    }
    else {
        if (m_MsgNrCmdList.contains(msgnr)) {
            int cmd = m_MsgNrCmdList.take(msgnr);
            switch (cmd) {
            case MODEMODINIT::setmode:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(setMeasModeErrMsg);
                break;

            case MODEMODINIT::writegaincorr:
            case MODEMODINIT::writegaincorr2:
                if (reply == ack) {
                    emit activationContinue();
                }
                else {
                    // perhaps we emit some error here ?
                    notifyError(writedspgaincorrErrMsg);
                }
                break;

            case MODEMODINIT::writephasecorr:
            case MODEMODINIT::writephasecorr2:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(writedspphasecorrErrMsg);
                break;

            case MODEMODINIT::writeoffsetcorr:
            case MODEMODINIT::writeoffsetcorr2:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(writedspoffsetcorrErrMsg);
                break;

            case MODEMODINIT::subdcdsp:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(writesubdcErrMsg);
                break;

            case MODEMODINIT::setsamplingsystem:
                if (reply == ack)
                    emit activationContinue();
                else
                    notifyError(setsamplingsystemErrmsg);
                break;

            case MODEMODINIT::freeresource:
                if (reply == ack || reply == nack) // we accept nack here also
                    emit deactivationContinue(); // maybe that resource was deleted by server and then it is no more set
                else
                    notifyError(freeresourceErrMsg);
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
    m_MsgNrCmdList[m_pcbInterface->setMMode(m_ConfigData.m_sMode)] = MODEMODINIT::setmode;
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
    m_pCorrectionDSP = m_dspInterface->getMemHandle("GainCorrection");
    m_pCorrectionDSP->addDspVar("GAINCORRECTION",32, DSPDATA::vDspIntVar);
    float* data = m_pCorrectionDSP->data("GAINCORRECTION");
    for (int i = 0; i < 32; i++) {
        data[i] = 1.0;
    }
    m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pCorrectionDSP)] = MODEMODINIT::writegaincorr;
}


void cModeModuleInit::writeGainCorr2()
{
    m_dspInterface->deleteMemHandle(m_pCorrectionDSP); // we delete the old handle

    m_pCorrectionDSP = m_dspInterface->getMemHandle("GainCorrection");
    m_pCorrectionDSP->addDspVar("GAINCORRECTION2",32, DSPDATA::vDspIntVar);
    float* data = m_pCorrectionDSP->data("GAINCORRECTION2");
    for (int i = 0; i < 32; i++) {
        data[i] = 1.0;
    }
    m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pCorrectionDSP)] = MODEMODINIT::writegaincorr2;
}


void cModeModuleInit::writePhaseCorr()
{
    m_dspInterface->deleteMemHandle(m_pCorrectionDSP); // we delete the old handle

    m_pCorrectionDSP = m_dspInterface->getMemHandle("PhaseCorrection");
    m_pCorrectionDSP->addDspVar("PHASECORRECTION",32, DSPDATA::vDspIntVar);
    float* data = m_pCorrectionDSP->data("PHASECORRECTION");
    for (int i = 0; i < 32; i++) {
        data[i] = 0.0;
    }
    m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pCorrectionDSP)] = MODEMODINIT::writephasecorr;
}


void cModeModuleInit::writePhaseCorr2()
{
    m_dspInterface->deleteMemHandle(m_pCorrectionDSP); // we delete the old handle

    m_pCorrectionDSP = m_dspInterface->getMemHandle("PhaseCorrection");
    m_pCorrectionDSP->addDspVar("PHASECORRECTION2",32, DSPDATA::vDspIntVar);
    float* data = m_pCorrectionDSP->data("PHASECORRECTION2");
    for (int i = 0; i < 32; i++) {
        data[i] = 0.0;
    }
    m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pCorrectionDSP)] = MODEMODINIT::writephasecorr2;
}


void cModeModuleInit::writeOffsetCorr()
{
    m_dspInterface->deleteMemHandle(m_pCorrectionDSP); // we delete the old handle

    m_pCorrectionDSP = m_dspInterface->getMemHandle("OffsetCorrection");
    m_pCorrectionDSP->addDspVar("OFFSETCORRECTION",32, DSPDATA::vDspIntVar);
    float* data = m_pCorrectionDSP->data("OFFSETCORRECTION");
    for (int i = 0; i < 32; i++) {
        data[i] = 0.0;
    }
    m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pCorrectionDSP)] = MODEMODINIT::writeoffsetcorr;
}


void cModeModuleInit::writeOffsetCorr2()
{
    m_dspInterface->deleteMemHandle(m_pCorrectionDSP); // we delete the old handle

    m_pCorrectionDSP = m_dspInterface->getMemHandle("OffsetCorrection");
    m_pCorrectionDSP->addDspVar("OFFSETCORRECTION2",32, DSPDATA::vDspIntVar);
    float* data = m_pCorrectionDSP->data("OFFSETCORRECTION2");
    for (int i = 0; i < 32; i++) {
        data[i] = 0.0;
    }
    m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(m_pCorrectionDSP)] = MODEMODINIT::writeoffsetcorr;
}


void cModeModuleInit::setSubDC()
{
    m_dspInterface->deleteMemHandle(m_pCorrectionDSP); // we delete the old handle

    quint32 subdc = 0;
    // here we can set if sub dc or not
    cDspMeasData* pSubDCMaskDSP = m_dspInterface->getMemHandle("SubDC");
    pSubDCMaskDSP->addDspVar("SUBDC",1, DSPDATA::vDspIntVar, DSPDATA::dInt);
    pSubDCMaskDSP->setVarData(QString("SUBDC:%1;").arg(subdc));
    m_MsgNrCmdList[m_dspInterface->dspMemoryWrite(pSubDCMaskDSP)] = MODEMODINIT::subdcdsp;
}


void cModeModuleInit::setSamplingsytem()
{
    m_MsgNrCmdList[m_dspInterface->setSamplingSystem(m_ConfigData.m_nChannelnr, m_ConfigData.m_nSignalPeriod, m_ConfigData.m_nMeasurePeriod)] = MODEMODINIT::setsamplingsystem;
}


void cModeModuleInit::activationDone()
{
    m_dspInterface->deleteMemHandle(m_pCorrectionDSP); // we delete the old handle
    emit activated();
}


void cModeModuleInit::deactivationDone()
{
    // and disconnect from our servers afterwards
    disconnect(m_pcbInterface.get(), 0, this, 0);
    emit deactivated();
}

}
