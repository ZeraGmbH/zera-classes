#include "scpiparameterdelegate.h"
#include "scpieventsystem.h"
#include "scpiclient.h"
#include <scpicommand.h>
#include <vcmp_componentdata.h>

namespace SCPIMODULE {

cSCPIParameterDelegate::cSCPIParameterDelegate(QString cmdParent, QString cmd, quint8 type, cSCPIModule *scpimodule,  cSCPICmdInfo* scpicmdinfo)
    :ScpiBaseDelegate(cmdParent, cmd, type), m_pModule(scpimodule), m_pSCPICmdInfo(scpicmdinfo)
{
}

cSCPIParameterDelegate::~cSCPIParameterDelegate()
{
    delete m_pSCPICmdInfo;
}

bool cSCPIParameterDelegate::executeSCPI(cSCPIClient *client, QString &sInput)
{
    quint8 scpiCmdType = getType();
    cSCPICommand cmd = sInput;
    bool bQuery = cmd.isQuery() || cmd.isQuery(1); // we allow queries without or with 1 parameter
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
        if (bQuery) {
            if (cmd.isQuery(1))
                cData->setNewValue(cmd.getParam(0));
            cData->setCommand(VeinComponent::ComponentData::Command::CCMD_FETCH);
        }
        else {
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
        SCPIClientInfoPtr clientinfo;
        if (bQuery)
            clientinfo = std::make_shared<cSCPIClientInfo>(client, m_pSCPICmdInfo->entityId, SCPIMODULE::parQuery);
        else
            clientinfo = std::make_shared<cSCPIClientInfo>(client, m_pSCPICmdInfo->entityId, SCPIMODULE::parcmd);

        m_pModule->scpiParameterCmdInfoHash.insert(m_pSCPICmdInfo->componentName, clientinfo);
        client->addSCPIClientInfo(m_pSCPICmdInfo->componentName, clientinfo);

        m_pModule->m_pSCPIEventSystem->sigSendEvent(event);
    }
    else {
        QMetaObject::Connection myConn = connect(this, &cSCPIParameterDelegate::signalStatus, client, &cSCPIClient::receiveStatus);
        emit signalStatus(SCPI::nak);
        disconnect(myConn);
    }
    return true;
}

}
