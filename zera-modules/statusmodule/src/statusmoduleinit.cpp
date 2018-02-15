#include <QFile>
#include <QTextStream>

#include <rminterface.h>
#include <pcbinterface.h>
#include <proxy.h>
#include <proxyclient.h>
#include <reply.h>
#include <errormessages.h>
#include <dspinterface.h>
#include <veinmoduleactvalue.h>
#include <modulevalidator.h>
#include <scpiinfo.h>

#include "debug.h"
#include "statusmodule.h"
#include "statusmoduleinit.h"
#include "statusmoduleconfigdata.h"


namespace STATUSMODULE
{

cStatusModuleInit::cStatusModuleInit(cStatusModule* module, Zera::Proxy::cProxy* proxy, cStatusModuleConfigData& configData)
    :m_pModule(module), m_pProxy(proxy), m_ConfigData(configData)
{
    m_pRMInterface = new Zera::Server::cRMInterface();
    m_pPCBInterface = new Zera::Server::cPCBInterface();
    m_pDSPInterface = new Zera::Server::cDSPInterface();

    m_IdentifyState.addTransition(this, SIGNAL(activationContinue()), &m_pcbserverConnectionState);
    // m_pcbserverConnectionState.addTransition is done in pcbserverConnection
    m_pcbserverReadVersionState.addTransition(this, SIGNAL(activationContinue()), &m_pcbserverReadCtrlVersionState);
    m_pcbserverReadCtrlVersionState.addTransition(this, SIGNAL(activationContinue()), &m_pcbserverReadFPGAVersionState);
    m_pcbserverReadFPGAVersionState.addTransition(this, SIGNAL(activationContinue()), &m_pcbserverReadSerialNrState);
    m_pcbserverReadSerialNrState.addTransition(this, SIGNAL(activationContinue()), &m_pcbserverReadAdjStatusState);
    m_pcbserverReadAdjStatusState.addTransition(this, SIGNAL(activationContinue()), &m_pcbserverReadAdjChksumState);
    m_pcbserverReadAdjChksumState.addTransition(this, SIGNAL(activationContinue()), &m_dspserverConnectionState);
    // m_dspserverConnectionState.addTransition is done in dspserverConnection
    m_dspserverReadVersionState.addTransition(this, SIGNAL(activationContinue()), &m_dspserverReadDSPProgramState);
    m_dspserverReadDSPProgramState.addTransition(this, SIGNAL(activationContinue()), &m_activationDoneState);


    m_activationMachine.addState(&m_resourceManagerConnectState);
    m_activationMachine.addState(&m_IdentifyState);
    m_activationMachine.addState(&m_pcbserverConnectionState);
    m_activationMachine.addState(&m_pcbserverReadVersionState);
    m_activationMachine.addState(&m_pcbserverReadCtrlVersionState);
    m_activationMachine.addState(&m_pcbserverReadFPGAVersionState);
    m_activationMachine.addState(&m_pcbserverReadSerialNrState);
    m_activationMachine.addState(&m_pcbserverReadAdjStatusState);
    m_activationMachine.addState(&m_pcbserverReadAdjChksumState);
    m_activationMachine.addState(&m_dspserverConnectionState);
    m_activationMachine.addState(&m_dspserverReadVersionState);
    m_activationMachine.addState(&m_dspserverReadDSPProgramState);
    m_activationMachine.addState(&m_activationDoneState);
    m_activationMachine.setInitialState(&m_resourceManagerConnectState);

    connect(&m_resourceManagerConnectState, SIGNAL(entered()), SLOT(resourceManagerConnect()));
    connect(&m_IdentifyState, SIGNAL(entered()), SLOT(sendRMIdent()));
    connect(&m_pcbserverConnectionState, SIGNAL(entered()), SLOT(pcbserverConnect()));
    connect(&m_pcbserverReadVersionState, SIGNAL(entered()), SLOT(pcbserverReadVersion()));
    connect(&m_pcbserverReadCtrlVersionState, SIGNAL(entered()), SLOT(pcbserverReadCtrlVersion()));
    connect(&m_pcbserverReadFPGAVersionState, SIGNAL(entered()), SLOT(pcbserverReadFPGAVersion()));
    connect(&m_pcbserverReadSerialNrState, SIGNAL(entered()), SLOT(pcbserverReadSerialNr()));
    connect(&m_pcbserverReadAdjStatusState, SIGNAL(entered()), SLOT(pcbserverReadAdjStatus()));
    connect(&m_pcbserverReadAdjChksumState, SIGNAL(entered()), SLOT(pcbserverReadAdjChksum()));
    connect(&m_dspserverConnectionState, SIGNAL(entered()), SLOT(dspserverConnect()));
    connect(&m_dspserverReadVersionState, SIGNAL(entered()), SLOT(dspserverReadVersion()));
    connect(&m_dspserverReadDSPProgramState, SIGNAL(entered()), SLOT(dspserverReadDSPProgramVersion()));
    connect(&m_activationDoneState, SIGNAL(entered()), SLOT(activationDone()));

    m_deactivationMachine.addState(&m_deactivationDoneState);
    m_deactivationMachine.setInitialState(&m_deactivationDoneState);
    connect(&m_deactivationDoneState, SIGNAL(entered()), SLOT(deactivationDone()));
}


cStatusModuleInit::~cStatusModuleInit()
{
    delete m_pRMInterface;
    delete m_pPCBInterface;
    delete m_pDSPInterface;
}


void cStatusModuleInit::generateInterface()
{
    m_pPCBServerVersion = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                  QString("INF_PCBServerVersion"),
                                                  QString("Component forwards the pcb server version"),
                                                  QVariant(QString("") ));

    m_pModule->veinModuleActvalueList.append(m_pPCBServerVersion);
    m_pPCBServerVersion->setSCPIInfo(new cSCPIInfo("STATUS", "VERSION:PCBSERVER", "2", "INF_PCBServerVersion", "0", ""));


    m_pCtrlVersion = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                              QString("INF_CTRLVersion"),
                                              QString("Component forwards the controler version"),
                                              QVariant(QString("") ));

    m_pModule->veinModuleActvalueList.append(m_pCtrlVersion);
    m_pCtrlVersion->setSCPIInfo(new cSCPIInfo("STATUS", "VERSION:CONTROLER", "2", "INF_CTRLVersion", "0", ""));


    m_pFPGAVersion = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                             QString("INF_FPGAVersion"),
                                             QString("Component forwards the fpga version"),
                                             QVariant(QString("") ));

    m_pModule->veinModuleActvalueList.append(m_pFPGAVersion);
    m_pFPGAVersion->setSCPIInfo(new cSCPIInfo("STATUS", "VERSION:FPGA", "2", "INF_FPGAVersion", "0", ""));


    m_pSerialNumber = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                              QString("INF_SerialNr"),
                                              QString("Component forwards the devices serial number"),
                                              QVariant(QString("") ));

    m_pModule->veinModuleActvalueList.append(m_pSerialNumber);
    m_pSerialNumber->setSCPIInfo(new cSCPIInfo("STATUS", "SERIAL", "2", "INF_SerialNr", "0", ""));


    m_pDSPServerVersion = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                  QString("INF_DSPServerVersion"),
                                                  QString("Component forwards the dsp server version"),
                                                  QVariant(QString("") ));

    m_pModule->veinModuleActvalueList.append(m_pDSPServerVersion);
    m_pDSPServerVersion->setSCPIInfo(new cSCPIInfo("STATUS", "VERSION:DSPSERVER", "2", "INF_DSPServerVersion", "0", ""));


    m_pDSPProgramVersion = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                   QString("INF_DSPVersion"),
                                                   QString("Component forwards the dsp software version"),
                                                   QVariant(QString("") ));

    m_pModule->veinModuleActvalueList.append(m_pDSPProgramVersion);
    m_pDSPProgramVersion->setSCPIInfo(new cSCPIInfo("STATUS", "VERSION:DSP", "2", "INF_DSPVersion", "0", ""));


    m_pReleaseNumber = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                               QString("INF_ReleaseNr"),
                                               QString("Component forwards the devices release number"),
                                               QVariant(QString("") ));

    m_pModule->veinModuleActvalueList.append(m_pReleaseNumber);
    m_pReleaseNumber->setSCPIInfo(new cSCPIInfo("STATUS", "RELEASE", "2", "INF_ReleaseNr", "0", ""));

    m_pAdjustmentStatus = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                QString("INF_Adjusted"),
                                                QString("Component forwards information about device adjustment"),
                                                QVariant(0));

    m_pModule->veinModuleActvalueList.append(m_pAdjustmentStatus);
    m_pAdjustmentStatus->setSCPIInfo(new cSCPIInfo("STATUS", "ADJUSTMENT", "2", "INF_Adjusted", "0", ""));

    m_pAdjustmentChksum = new cVeinModuleActvalue(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                                  QString("INF_AdjChksum"),
                                                  QString("Component forwards checksum of device adjustment data"),
                                                  QVariant(0));

    m_pModule->veinModuleActvalueList.append(m_pAdjustmentChksum);
    m_pAdjustmentChksum->setSCPIInfo(new cSCPIInfo("STATUS", "ADJCHKSUM", "2", "INF_AdjChksum", "0", ""));
}


void cStatusModuleInit::deleteInterface()
{
    // we have nothing to delete ...will be done if basemodule is destroyed
}


void cStatusModuleInit::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if (msgnr == 0) // 0 was reserved for async. messages
    {
        // that we will ignore
    }
    else
    {
        if (m_MsgNrCmdList.contains(msgnr))
        {
            int cmd = m_MsgNrCmdList.take(msgnr);
            switch (cmd)
            {
            case STATUSMODINIT::sendrmident:
                if (reply == ack) // we only continue if resource manager acknowledges
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(rmidentErrMSG)));
#ifdef DEBUG
                    qDebug() << rmidentErrMSG;
#endif
                    emit activationError();
                }
                break;

            case STATUSMODINIT::readPCBServerVersion:
                if (reply == ack)
                {
                    m_sPCBServerVersion = answer.toString();
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readPCBServerVersionErrMsg)));
#ifdef DEBUG
                    qDebug() << readPCBServerVersionErrMsg;
#endif
                    emit activationError();
                }
                break;

            case STATUSMODINIT::readPCBServerCtrlVersion:
                if (reply == ack)
                {
                    m_sCtrlVersion = answer.toString();
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readPCBCtrlVersionErrMSG)));
#ifdef DEBUG
                    qDebug() << readPCBCtrlVersionErrMSG;
#endif
                    emit activationError();
                }
                break;

            case STATUSMODINIT::readPCBServerFPGAVersion:
                if (reply == ack)
                {
                    m_sFPGAVersion = answer.toString();
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readPCBFPGAVersionErrMSG)));
#ifdef DEBUG
                    qDebug() << readPCBFPGAVersionErrMSG;
#endif
                    emit activationError();
                }
                break;


            case STATUSMODINIT::readPCBServerSerialNumber:
                if (reply == ack)
                {
                    m_sSerialNumber = answer.toString();
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readPCBSerialNrErrMSG)));
#ifdef DEBUG
                    qDebug() << readPCBSerialNrErrMSG;
#endif
                    emit activationError();
                }
                break;

            case STATUSMODINIT::readPCBServerAdjStatus:
                if (reply == ack)
                {
                    m_sAdjStatus = answer.toString();
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readadjstatusErrMsg)));
#ifdef DEBUG
                    qDebug() << readadjstatusErrMsg;
#endif
                    emit activationError();
                }
                break;

            case STATUSMODINIT::readPCBServerAdjChksum:
                if (reply == ack)
                {
                    m_sAdjChksum = answer.toString();
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readadjchksumErrMsg)));
#ifdef DEBUG
                    qDebug() << readadjchksumErrMsg;
#endif
                    emit activationError();
                }
                break;

            case STATUSMODINIT::readDSPServerVersion:
                if (reply == ack)
                {
                    m_sDSPServerVersion = answer.toString();
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readDSPServerVersionErrMsg)));
#ifdef DEBUG
                    qDebug() << readDSPServerVersionErrMsg;
#endif
                    emit activationError();
                }
                break;

            case STATUSMODINIT::readDSPServerDSPProgramVersion:
                if (reply == ack)
                {
                    m_sDSPProgramVersion = answer.toString();
                    emit activationContinue();
                }
                else
                {
                    emit errMsg((tr(readDSPProgramVersionErrMsg)));
#ifdef DEBUG
                    qDebug() << readDSPProgramVersionErrMsg;
#endif
                    emit activationError();
                }
                break;
            }
        }
    }
}


QString cStatusModuleInit::findReleaseNr()
{
    QString releaseNr;

    releaseNr = m_pModule->getReleaseNr(ReleaseInfoFilePath);
    if (releaseNr == "")
        emit errMsg(releaseNumberErrMsg);

    return releaseNr;
}


void cStatusModuleInit::setInterfaceComponents()
{
    m_pPCBServerVersion->setValue(QVariant(m_sPCBServerVersion));
    m_pCtrlVersion->setValue(QVariant(m_sCtrlVersion));
    m_pFPGAVersion->setValue(QVariant(m_sFPGAVersion));
    m_pSerialNumber->setValue(QVariant(m_sSerialNumber));
    m_pDSPServerVersion->setValue((QVariant(m_sDSPServerVersion)));
    m_pDSPProgramVersion->setValue(QVariant(m_sDSPProgramVersion));
    m_pReleaseNumber->setValue(QVariant(m_sReleaseNumber));
    m_pAdjustmentStatus->setValue(QVariant(m_sAdjStatus));
    m_pAdjustmentChksum->setValue(QVariant(m_sAdjChksum));
}


void cStatusModuleInit::resourceManagerConnect()
{
    // first we try to get a connection to resource manager over proxy
    m_pRMClient = m_pProxy->getConnection(m_ConfigData.m_RMSocket.m_sIP, m_ConfigData.m_RMSocket.m_nPort);
    // and then we set connection resource manager interface's connection
    m_pRMInterface->setClient(m_pRMClient); //
    m_resourceManagerConnectState.addTransition(m_pRMClient, SIGNAL(connected()), &m_IdentifyState);
    connect(m_pRMInterface, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    // todo insert timer for timeout and/or connect error conditions
    m_pProxy->startConnection(m_pRMClient);
}


void cStatusModuleInit::sendRMIdent()
{
    m_MsgNrCmdList[m_pRMInterface->rmIdent(QString("VersionModuleInit%1").arg(m_pModule->getModuleNr()))] = STATUSMODINIT::sendrmident;
}


void cStatusModuleInit::pcbserverConnect()
{
    m_pPCBClient = m_pProxy->getConnection(m_ConfigData.m_PCBServerSocket.m_sIP, m_ConfigData.m_PCBServerSocket.m_nPort);
    m_pcbserverConnectionState.addTransition(m_pPCBClient, SIGNAL(connected()), &m_pcbserverReadVersionState);

    m_pPCBInterface->setClient(m_pPCBClient);
    connect(m_pPCBInterface, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    m_pProxy->startConnection(m_pPCBClient);
}


void cStatusModuleInit::pcbserverReadVersion()
{
    m_MsgNrCmdList[m_pPCBInterface->readServerVersion()] = STATUSMODINIT::readPCBServerVersion;
}


void cStatusModuleInit::pcbserverReadCtrlVersion()
{
    m_MsgNrCmdList[m_pPCBInterface->readCTRLVersion()] = STATUSMODINIT::readPCBServerCtrlVersion;
}


void cStatusModuleInit::pcbserverReadFPGAVersion()
{
    m_MsgNrCmdList[m_pPCBInterface->readFPGAVersion()] = STATUSMODINIT::readPCBServerFPGAVersion;
}


void cStatusModuleInit::pcbserverReadSerialNr()
{
    m_MsgNrCmdList[m_pPCBInterface->readSerialNr()] = STATUSMODINIT::readPCBServerSerialNumber;
}


void cStatusModuleInit::pcbserverReadAdjStatus()
{
    m_MsgNrCmdList[m_pPCBInterface->getAdjustmentStatus()] = STATUSMODINIT::readPCBServerAdjStatus;
}


void cStatusModuleInit::pcbserverReadAdjChksum()
{
    m_MsgNrCmdList[m_pPCBInterface->getAdjustmentChksum()] = STATUSMODINIT::readPCBServerAdjChksum;
}


void cStatusModuleInit::dspserverConnect()
{
    // we set up our dsp server connection
    m_pDSPClient = m_pProxy->getConnection(m_ConfigData.m_DSPServerSocket.m_sIP, m_ConfigData.m_DSPServerSocket.m_nPort);
    m_pDSPInterface->setClient(m_pDSPClient);
    m_dspserverConnectionState.addTransition(m_pDSPClient, SIGNAL(connected()), &m_dspserverReadVersionState);
    connect(m_pDSPInterface, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    m_pProxy->startConnection(m_pDSPClient);
}


void cStatusModuleInit::dspserverReadVersion()
{
    m_MsgNrCmdList[m_pDSPInterface->readServerVersion()] = STATUSMODINIT::readDSPServerVersion;
}


void cStatusModuleInit::dspserverReadDSPProgramVersion()
{
    m_MsgNrCmdList[m_pDSPInterface->readDeviceVersion()] = STATUSMODINIT::readDSPServerDSPProgramVersion;
}


void cStatusModuleInit::activationDone()
{
    m_sReleaseNumber = findReleaseNr();
    setInterfaceComponents();
    emit activated();
}


void cStatusModuleInit::deactivationDone()
{
    m_pProxy->releaseConnection(m_pRMClient);
    m_pProxy->releaseConnection(m_pDSPClient);
    m_pProxy->releaseConnection(m_pPCBClient);
    // and disconnect from our servers afterwards
    disconnect(m_pRMInterface, 0, this, 0);
    disconnect(m_pDSPInterface, 0, this, 0);
    disconnect(m_pPCBInterface, 0, this, 0);
    emit deactivated();
}

}
