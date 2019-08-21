#include <scpi.h>
#include <scpicommand.h>
#include <ve_storagesystem.h>
#include <vcmp_componentdata.h>
#include <ve_commandevent.h>
#include <ve_eventsystem.h>

#include "scpimodule.h"
#include "scpicmdinfo.h"
#include "scpiclientinfo.h"
#include "scpiparameterdelegate.h"
#include "scpieventsystem.h"
#include "scpiclient.h"


namespace SCPIMODULE
{

cSCPIParameterDelegate::cSCPIParameterDelegate(QString cmdParent, QString cmd, quint8 type, cSCPIModule *scpimodule,  cSCPICmdInfo* scpicmdinfo)
    :cSCPIDelegate(cmdParent, cmd, type), m_pModule(scpimodule), m_pSCPICmdInfo(scpicmdinfo)
{
}


cSCPIParameterDelegate::~cSCPIParameterDelegate()
{
    delete m_pSCPICmdInfo;
}


bool cSCPIParameterDelegate::executeSCPI(cSCPIClient *client, QString &sInput)
{
    quint8 scpiCmdType;
    bool bQuery;

    scpiCmdType = getType();
    cSCPICommand cmd = sInput;

    bQuery = cmd.isQuery() || cmd.isQuery(1); // we allow queries without or with 1 parameter

    if ( (bQuery && ((scpiCmdType & SCPI::isQuery) > 0)) ||  // test if we got an allowed query
         (cmd.isCommand(1) && ((scpiCmdType & SCPI::isCmdwP) > 0)) ||  // test if we got an allowed cmd + 1 parameter
         ((scpiCmdType & SCPI::isXMLCmd) > 0) ) // test if we expext an xml command
    {
        VeinComponent::ComponentData *cData;

        cData = new VeinComponent::ComponentData();
        cData->setEntityId(m_pSCPICmdInfo->entityId);
        cData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
        cData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
        cData->setComponentName(m_pSCPICmdInfo->componentName);
        cData->setOldValue(m_pModule->m_pStorageSystem->getStoredValue(m_pSCPICmdInfo->entityId, m_pSCPICmdInfo->componentName));

        if (bQuery)
            cData->setCommand(VeinComponent::ComponentData::Command::CCMD_FETCH);

        else
        {
            if ((scpiCmdType & SCPI::isXMLCmd) > 0)
                cData->setNewValue(cmd.getParam()); // if we expect an xml command we take all text behind the command
            else
                cData->setNewValue(cmd.getParam(0));

            cData->setCommand(VeinComponent::ComponentData::Command::CCMD_SET);
        }


        VeinEvent::CommandEvent *event;
        event = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::TRANSACTION, cData);
        event->setPeerId(client->getClientId());

        // we memorize : for component (componentname) the client to set something
        cSCPIClientInfo *clientinfo;
        if (bQuery)
            clientinfo = new cSCPIClientInfo(client, m_pSCPICmdInfo->entityId, SCPIMODULE::parQuery);
        else
            clientinfo = new cSCPIClientInfo(client, m_pSCPICmdInfo->entityId, SCPIMODULE::parcmd);

        m_pModule->scpiParameterCmdInfoHash.insert(m_pSCPICmdInfo->componentName, clientinfo);

        QMetaObject::Connection myConn = connect(this, SIGNAL(clientinfoSignal(QString,cSCPIClientInfo*)), client, SLOT(addSCPIClientInfo(QString,cSCPIClientInfo*)));
        emit clientinfoSignal(m_pSCPICmdInfo->componentName, clientinfo);
        disconnect(myConn);

        m_pModule->m_pSCPIEventSystem->sigSendEvent(event);

    }

    /*
    if (cmd.isQuery() && ((scpiCmdType & SCPI::isQuery) > 0)) // test if we got an allowed query
    {
        QMetaObject::Connection myConn = connect(this, SIGNAL(signalAnswer(QString)), client, SLOT(receiveAnswer(QString)));
        QString answer = m_pModule->m_pStorageSystem->getStoredValue(m_pSCPICmdInfo->entityId, m_pSCPICmdInfo->componentName).toString();
        emit signalAnswer(answer);
        disconnect(myConn);
        //client->receiveAnswer(answer);
    }

    else

        if ( (cmd.isCommand(1) && ((scpiCmdType & SCPI::isCmdwP) > 0)) ||  // test if we got an allowed cmd + 1 parameter
             (cmd.isQuery(1) && ((scpiCmdType & SCPI::isQuery) > 0)) )     // test if we got an allowed query + 1 parameter
        {
            VeinComponent::ComponentData *cData;
            QVariant oldValue = m_pModule->m_pStorageSystem->getStoredValue(m_pSCPICmdInfo->entityId, m_pSCPICmdInfo->componentName);
            QVariant newValue = cmd.getParam(0);
            // newValue.convert(oldValue.type());

            cData = new VeinComponent::ComponentData();

            cData->setEntityId(m_pSCPICmdInfo->entityId);
            cData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
            cData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
            cData->setCommand(VeinComponent::ComponentData::Command::CCMD_SET);
            cData->setComponentName(m_pSCPICmdInfo->componentName);
            cData->setOldValue(oldValue);
            cData->setNewValue(newValue);

            VeinEvent::CommandEvent *event;
            event = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::TRANSACTION, cData);
            event->setPeerId(client->getClientId());

            // we memorize : for component (componentname) the client to set something
            cSCPIClientInfo *clientinfo;
            if (cmd.isCommand(1))
                clientinfo = new cSCPIClientInfo(client, m_pSCPICmdInfo->entityId, SCPIMODULE::parcmd);
            else
                clientinfo = new cSCPIClientInfo(client, m_pSCPICmdInfo->entityId, SCPIMODULE::parQuery);
            m_pModule->scpiParameterCmdInfoHash.insert(m_pSCPICmdInfo->componentName, clientinfo);

            QMetaObject::Connection myConn = connect(this, SIGNAL(clientinfoSignal(QString,cSCPIClientInfo*)), client, SLOT(addSCPIClientInfo(QString,cSCPIClientInfo*)));
            emit clientinfoSignal(m_pSCPICmdInfo->componentName, clientinfo);
            disconnect(myConn);

            m_pModule->m_pSCPIEventSystem->sigSendEvent(event);
        }
        */

    else
    {
        QMetaObject::Connection myConn = connect(this, SIGNAL(signalStatus(quint8)), client, SLOT(receiveStatus(quint8)));
        emit signalStatus(SCPI::nak);
        disconnect(myConn);
        //client->receiveStatus(SCPI::nak);
    }

    return true;
}

}
