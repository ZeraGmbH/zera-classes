#include <rminterface.h>
#include <pcbinterface.h>
#include <proxy.h>
#include <proxyclient.h>
#include <reply.h>
#include <errormessages.h>

#include "modemodule.h"
#include "modemoduleinit.h"
#include "modemoduleconfigdata.h"


namespace MODEMODULE
{

cModeModuleInit::cModeModuleInit(cModeModule *module, Zera::Proxy::cProxy *proxy, VeinPeer *peer, cModeModuleConfigData &configData)
    :m_pModule(module), m_pProxy(proxy), m_pPeer(peer), m_ConfigData(configData)
{
    m_pRMInterface = new Zera::Server::cRMInterface();
    m_pPCBInterface = new Zera::Server::cPCBInterface();

    m_IdentifyState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceTypesState);
    m_readResourceTypesState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceState);
    m_readResourceState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceInfoState);
    m_readResourceInfoState.addTransition(this, SIGNAL(activationContinue()), &m_claimResourceState);
    m_claimResourceState.addTransition(this, SIGNAL(activationContinue()), &m_pcbserverConnectionState);
    // m_pcbserverConnectionState.addTransition is done in pcbserverConnection
    m_setModeState.addTransition(this, SIGNAL(activationContinue()), &m_activationDoneState);
    m_activationMachine.addState(&m_resourceManagerConnectState);
    m_activationMachine.addState(&m_IdentifyState);
    m_activationMachine.addState(&m_readResourceTypesState);
    m_activationMachine.addState(&m_readResourceState);
    m_activationMachine.addState(&m_readResourceInfoState);
    m_activationMachine.addState(&m_claimResourceState);
    m_activationMachine.addState(&m_pcbserverConnectionState);
    m_activationMachine.addState(&m_setModeState);
    m_activationMachine.addState(&m_activationDoneState);
    m_activationMachine.setInitialState(&m_resourceManagerConnectState);
    connect(&m_resourceManagerConnectState, SIGNAL(entered()), SLOT(resourceManagerConnect()));
    connect(&m_IdentifyState, SIGNAL(entered()), SLOT(sendRMIdent()));
    connect(&m_readResourceTypesState, SIGNAL(entered()), SLOT(readResourceTypes()));
    connect(&m_readResourceState, SIGNAL(entered()), SLOT(readResource()));
    connect(&m_readResourceInfoState, SIGNAL(entered()), SLOT(readResourceInfo()));
    connect(&m_claimResourceState, SIGNAL(entered()), SLOT(claimResource()));
    connect(&m_pcbserverConnectionState, SIGNAL(entered()), SLOT(pcbserverConnect()));
    connect(&m_setModeState, SIGNAL(entered()), SLOT(setMode()));
    connect(&m_activationDoneState, SIGNAL(entered()), SLOT(activationDone()));

    m_freeResourceState.addTransition(this, SIGNAL(deactivationContinue()), &m_deactivationDoneState);
    m_deactivationMachine.addState(&m_freeResourceState);
    m_deactivationMachine.addState(&m_deactivationDoneState);
    m_deactivationMachine.setInitialState(&m_freeResourceState);
    connect(&m_freeResourceState, SIGNAL(entered()), SLOT(freeResource()));
    connect(&m_deactivationDoneState, SIGNAL(entered()), SLOT(deactivationDone()));
}


cModeModuleInit::~cModeModuleInit()
{
    delete m_pRMInterface;
    delete m_pPCBInterface;
}


void cModeModuleInit::generateInterface()
{
    // at the moment we have no interface
}


void cModeModuleInit::deleteInterface()
{
    // so we have nothing to delete
}


void cModeModuleInit::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
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
            case MODEMODINIT::sendrmident:
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

            case MODEMODINIT::readresourcetypes:
                if ((reply == ack) && (answer.toString().contains("SENSE")))
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

            case MODEMODINIT::readresource:
                if ((reply == ack) && (answer.toString().contains("MMODE")))
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

            case MODEMODINIT::readresourceinfo:
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

            case MODEMODINIT::claimresource:
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

            case MODEMODINIT::setmode:
                if (reply == ack)
                    emit activationContinue();
                else
                {
                    emit errMsg((tr(setMeasModeErrMsg)));
#ifdef DEBUG
                    qDebug() << setMeasModeErrMsg;
#endif
                    emit activationError();
                }
                break;

            case MODEMODINIT::freeresource:
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
            }
        }
    }
}


void cModeModuleInit::resourceManagerConnect()
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


void cModeModuleInit::sendRMIdent()
{
    m_MsgNrCmdList[m_pRMInterface->rmIdent(QString("ModeModuleInit%1").arg(m_pModule->getModuleNr()))] = MODEMODINIT::sendrmident;
}


void cModeModuleInit::readResourceTypes()
{
    m_MsgNrCmdList[m_pRMInterface->getResourceTypes()] = MODEMODINIT::readresourcetypes;
}


void cModeModuleInit::readResource()
{
    m_MsgNrCmdList[m_pRMInterface->getResources("SENSE")] = MODEMODINIT::readresource;
}


void cModeModuleInit::readResourceInfo()
{
    m_MsgNrCmdList[m_pRMInterface->getResourceInfo("SENSE", "MMODE")] = MODEMODINIT::readresourceinfo;
}


void cModeModuleInit::claimResource()
{
    m_MsgNrCmdList[m_pRMInterface->setResource("SENSE", "MMODE", 1)] = MODEMODINIT::claimresource;
}


void cModeModuleInit::pcbserverConnect()
{
    m_pPCBClient = m_pProxy->getConnection(m_ConfigData.m_PCBServerSocket.m_sIP, m_nPort);
    m_pcbserverConnectionState.addTransition(m_pPCBClient, SIGNAL(connected()), &m_setModeState);

    m_pPCBInterface->setClient(m_pPCBClient);
    connect(m_pPCBInterface, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    m_pProxy->startConnection(m_pPCBClient);
}


void cModeModuleInit::setMode()
{
    m_MsgNrCmdList[m_pPCBInterface->setMMode(m_ConfigData.m_sMode)] = MODEMODINIT::setmode;
}


void cModeModuleInit::activationDone()
{
    emit activated();
}


void cModeModuleInit::freeResource()
{
    m_pProxy->releaseConnection(m_pPCBClient);
    m_MsgNrCmdList[m_pRMInterface->freeResource("SENSE", "MMODE")] = MODEMODINIT::freeresource;
}


void cModeModuleInit::deactivationDone()
{
    m_pProxy->releaseConnection(m_pRMClient);
    // and disconnect from our servers afterwards
    disconnect(m_pRMInterface, 0, this, 0);
    disconnect(m_pPCBInterface, 0, this, 0);
    emit deactivated();
}

}
