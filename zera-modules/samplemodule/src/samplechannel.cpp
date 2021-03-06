#include <QRegExp>
#include <rminterface.h>
#include <pcbinterface.h>
#include <proxy.h>
#include <proxyclient.h>
#include <scpiinfo.h>
#include <modulevalidator.h>
#include <stringvalidator.h>
#include <scpiinfo.h>
#include <veinmoduleparameter.h>

#include "debug.h"
#include "errormessages.h"
#include "samplechannel.h"
#include "samplemodule.h"
#include "samplemoduleconfigdata.h"

namespace SAMPLEMODULE
{

cSampleChannel::cSampleChannel(cSampleModule* module,Zera::Proxy::cProxy* proxy, cSampleModuleConfigData& configdata, quint8 chnnr)
    :m_pModule(module), m_ConfigData(configdata), cBaseSampleChannel(proxy, configdata.m_ObsermaticConfPar.m_sSampleSystem, chnnr)
{
    m_pRMInterface = new Zera::Server::cRMInterface();
    m_pPCBInterface = new Zera::Server::cPCBInterface();

    // setting up statemachine for "activating" sample channel
    // m_rmConnectState.addTransition is done in rmConnect
    m_IdentifyState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceTypesState);
    m_readResourceTypesState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceState);
    m_readResourceState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceInfoState);
    m_readResourceInfoState.addTransition(this, SIGNAL(activationContinue()), &m_claimResourceState);
    m_claimResourceState.addTransition(this, SIGNAL(activationContinue()), &m_pcbConnectionState);
    // m_pcbConnectionState.addTransition is done in pcbConnection
    m_readChnAliasState.addTransition(this, SIGNAL(activationContinue()), &m_readRangelistState);
    m_readRangelistState.addTransition(this, SIGNAL(activationContinue()), &m_activationDoneState);

    m_activationMachine.addState(&m_rmConnectState);
    m_activationMachine.addState(&m_IdentifyState);
    m_activationMachine.addState(&m_readResourceTypesState);
    m_activationMachine.addState(&m_readResourceState);
    m_activationMachine.addState(&m_readResourceInfoState);
    m_activationMachine.addState(&m_claimResourceState);
    m_activationMachine.addState(&m_pcbConnectionState);
    m_activationMachine.addState(&m_readChnAliasState);

    m_activationMachine.addState(&m_readRangelistState);
    m_activationMachine.addState(&m_activationDoneState);
    m_activationMachine.setInitialState(&m_rmConnectState);

    connect(&m_rmConnectState, SIGNAL(entered()), SLOT(rmConnect()));
    connect(&m_IdentifyState, SIGNAL(entered()), SLOT(sendRMIdent()));
    connect(&m_readResourceTypesState, SIGNAL(entered()), SLOT(readResourceTypes()));
    connect(&m_readResourceState, SIGNAL(entered()), SLOT(readResource()));
    connect(&m_readResourceInfoState, SIGNAL(entered()), SLOT(readResourceInfo()));
    connect(&m_claimResourceState, SIGNAL(entered()), SLOT(claimResource()));
    connect(&m_pcbConnectionState, SIGNAL(entered()), SLOT(pcbConnection()));
    connect(&m_readChnAliasState, SIGNAL(entered()), SLOT(readChnAlias()));
    connect(&m_readRangelistState, SIGNAL(entered()), SLOT(readRangelist()));
    connect(&m_activationDoneState, SIGNAL(entered()), SLOT(activationDone()));

    // setting up statemachine for "deactivating" meas channel
    m_deactivationInitState.addTransition(this, SIGNAL(deactivationContinue()), &m_deactivationDoneState);
    m_deactivationMachine.addState(&m_deactivationInitState);
    m_deactivationMachine.addState(&m_deactivationDoneState);
    m_deactivationMachine.setInitialState(&m_deactivationInitState);
    connect(&m_deactivationInitState, SIGNAL(entered()), SLOT(deactivationInit()));
    connect(&m_deactivationDoneState, SIGNAL(entered()), SLOT(deactivationDone()));
}


cSampleChannel::~cSampleChannel()
{
    m_pProxy->releaseConnection(m_pRMClient);
    m_pProxy->releaseConnection(m_pPCBClient);
    delete m_pRMInterface;
    delete m_pPCBInterface;
}


void cSampleChannel::generateInterface()
{
    QString key;
    cSCPIInfo *scpiInfo;

    m_pChannelRange = new cVeinModuleParameter(m_pModule->m_nEntityId, m_pModule->m_pModuleValidator,
                                               key = QString("PAR_ChannelRange"),
                                               QString("Component for reading and setting the sampling channel's range"),
                                               QVariant(m_ConfigData.m_ObsermaticConfPar.m_pllRange.m_sPar));

    m_pModule->veinModuleParameterHash[key] = m_pChannelRange; // for modules use

    scpiInfo = new cSCPIInfo("CONFIGURATION", "SRANGE", "10", m_pChannelRange->getName(), "0", "");
    m_pChannelRange->setSCPIInfo(scpiInfo);
    // later we still have to set the validator for this component
}



void cSampleChannel::deleteInterface()
{
}


void cSampleChannel::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    int cmd = m_MsgNrCmdList.take(msgnr);

    switch (cmd)
    {
    case sendsamplechannelrmident:
        if (reply == ack) // we only continue if resource manager acknowledges
            emit activationContinue();
        else
        {
            emit errMsg(tr(rmidentErrMSG));
#ifdef DEBUG
            qDebug() << rmidentErrMSG;
#endif
            emit activationError();
        }
        break;
    case readresourcetypessamplechannel:
        if ((reply == ack) && (answer.toString().contains("SAMPLE")))
            emit activationContinue();
        else
        {
            emit errMsg((tr(resourcetypeErrMsg)));
#ifdef DEBUG
            qDebug() << resourcetypeErrMsg;
#endif
            emit activationError();
        }
        break;
    case readresourcesamplechannel:
        if ((reply == ack) && (answer.toString().contains(m_sName)))
            emit activationContinue();
        else
        {
            emit errMsg((tr(resourceErrMsg)));
#ifdef DEBUG
            qDebug() << resourceErrMsg;
#endif
            emit activationError();
        }
        break;
    case readresourceinfosamplechannel:
    {
        bool ok1, ok2, ok3;
        int max, free;
        QStringList sl;


        sl = answer.toString().split(';');
        if ((reply ==ack) && (sl.length() >= 4))
        {
            max = sl.at(0).toInt(&ok1); // fixed position
            free = sl.at(1).toInt(&ok2);
            m_sDescription = sl.at(2);
            m_nPort = sl.at(3).toInt(&ok3);

            if (ok1 && ok2 && ok3 && ((max == free) == 1))
            {
                emit activationContinue();
            }

            else
            {
                emit errMsg((tr(resourceInfoErrMsg)));
#ifdef DEBUG
                qDebug() << resourceInfoErrMsg;
#endif
                emit activationError();
            }
        }

        else
        {
            emit errMsg((tr(resourceInfoErrMsg)));
#ifdef DEBUG
            qDebug() << resourceInfoErrMsg;
#endif
            emit activationError();
        }

        break;

    }
    case claimresource:
        if (reply == ack)
            emit activationContinue();
        else
        {
            emit errMsg((tr(claimresourceErrMsg)));
#ifdef DEBUG
            qDebug() << claimresourceErrMsg;
#endif
            emit activationError();
        }
        break;
    case freeresource:
        if (reply == ack || reply == nack) // we accept nack here also
            emit deactivationContinue(); // maybe that resource was deleted by server and then it is no more set
        else
        {
            emit errMsg((tr(freeresourceErrMsg)));
#ifdef DEBUG
            qDebug() << freeresourceErrMsg;
#endif
            emit deactivationError();
        }
        break;
    case readchnaliassamplechannel:
        if (reply == ack)
        {
            m_sAlias = answer.toString();
            emit activationContinue();
        }
        else
        {
            emit errMsg((tr(readaliasErrMsg)));
#ifdef DEBUG
            qDebug() << readaliasErrMsg;
#endif
            emit activationError();
        }
        break;
    case readrangelistsamplechannel:
        if (reply == ack)
        {
            m_RangeNameList = answer.toStringList();
            emit activationContinue();
        }
        else
        {
            emit errMsg((tr(readrangelistErrMsg)));
#ifdef DEBUG
            qDebug() << readrangelistErrMsg;
#endif
            emit activationError();
        }
        break;
    case setsamplechannelrange:
        if (reply == ack)
            m_sActRange = m_sNewRange;
        else
        {
            emit errMsg((tr(setRangeErrMsg)));
#ifdef DEBUG
            qDebug() << setRangeErrMsg;
#endif
            emit executionError();
        }; // perhaps some error output
        break;
    }
}


void cSampleChannel::setRangeValidator()
{
    cStringValidator *sValidator;

    sValidator = new cStringValidator(m_RangeNameList);
    m_pChannelRange->setValidator(sValidator);
}


void cSampleChannel::setChannelNameMetaInfo()
{
    QString s,s1,s2;

    s1 = s2 = m_sAlias;
    s1.remove(QRegExp("[1-9][0-9]?"));
    s2.remove(s1);
    //m_pChannelEntity->setValue(m_sAlias, m_pPeer);

    s = s1 + "%1" + QString(";%1;[%2]").arg(s2).arg("");
    // m_pChannelEntity->setValue(s, m_pPeer);
}


void cSampleChannel::rmConnect()
{
    // we instantiate a working resource manager interface first
    // so first we try to get a connection to resource manager over proxy
    m_pRMClient = m_pProxy->getConnection(m_ConfigData.m_RMSocket.m_sIP, m_ConfigData.m_RMSocket.m_nPort);
    m_rmConnectState.addTransition(m_pRMClient, SIGNAL(connected()), &m_IdentifyState);
    // and then we set connection resource manager interface's connection
    m_pRMInterface->setClient(m_pRMClient); //
    // todo insert timer for timeout

    connect(m_pRMInterface, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    m_pProxy->startConnection(m_pRMClient);
    // resource manager liste sense abfragen
    // bin ich da drin ?
    // nein -> fehler activierung
    // ja -> socket von rm besorgen
    // resource bei rm belegen
    // beim pcb proxy server interface beantragen

    // quint8 m_nDspChannel; dsp kanal erfragen
    // QString m_sAlias; kanal alias erfragen
    // eine liste aller möglichen bereichen erfragen
}


void cSampleChannel::sendRMIdent()
{
   m_MsgNrCmdList[m_pRMInterface->rmIdent(QString("SampleChannel%1").arg(m_nChannelNr))] = sendsamplechannelrmident;
}


void cSampleChannel::readResourceTypes()
{
    m_MsgNrCmdList[m_pRMInterface->getResourceTypes()] = readresourcetypessamplechannel;
}


void cSampleChannel::readResource()
{
    m_MsgNrCmdList[m_pRMInterface->getResources("SAMPLE")] = readresourcesamplechannel;
}


void cSampleChannel::readResourceInfo()
{
    m_MsgNrCmdList[m_pRMInterface->getResourceInfo("SAMPLE", m_sName)] = readresourceinfosamplechannel;
}


void cSampleChannel::claimResource()
{
    m_MsgNrCmdList[m_pRMInterface->setResource("SAMPLE", m_sName, 1)] = claimresource;
}


void cSampleChannel::pcbConnection()
{
    m_pPCBClient = m_pProxy->getConnection(m_ConfigData.m_PCBServerSocket.m_sIP, m_ConfigData.m_PCBServerSocket.m_nPort);
    m_pcbConnectionState.addTransition(m_pPCBClient, SIGNAL(connected()), &m_readChnAliasState);

    m_pPCBInterface->setClient(m_pPCBClient);
    connect(m_pPCBInterface, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    m_pProxy->startConnection(m_pPCBClient);
}


void cSampleChannel::readChnAlias()
{
    m_MsgNrCmdList[m_pPCBInterface->getAliasSample(m_sName)] = readchnaliassamplechannel;
}


void cSampleChannel::readRangelist()
{
    m_MsgNrCmdList[m_pPCBInterface->getRangeListSample(m_sName)] = readrangelistsamplechannel;
}


void cSampleChannel::activationDone()
{
    setChannelNameMetaInfo(); // we set our real name now
    setRangeValidator(); // and the list of possible ranges

    connect(m_pChannelRange, SIGNAL(sigValueChanged(QVariant)), this, SLOT(newPLLRange(QVariant)));
    emit activated();
}


void cSampleChannel::deactivationInit()
{
    // deactivation means we have to free our resources
    m_MsgNrCmdList[m_pRMInterface->freeResource("SAMPLE", m_sName)] = freeresource;
}


void cSampleChannel::deactivationDone()
{
    // and disconnect for our servers afterwards
    disconnect(m_pRMInterface, 0, this, 0);
    disconnect(m_pPCBInterface, 0, this, 0);
    emit deactivated();
}


void cSampleChannel::newPLLRange(QVariant rng)
{
    m_sNewRange = rng.toString();
    // m_pPllSignal->m_pParEntity->setValue(QVariant(1), m_pPeer); // we signal that we are changing pll channel
    m_MsgNrCmdList[m_pPCBInterface->setRangeSample(m_sName, m_sNewRange)] = setsamplechannelrange;
}


}
