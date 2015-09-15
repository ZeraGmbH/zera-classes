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

    scpiCmdType = getType();
    cSCPICommand cmd = sInput;

    QMetaObject::Connection myConn = connect(this, SIGNAL(signalAnswer(QString)), client, SLOT(receiveAnswer(QString)));

    if (cmd.isQuery() && ((scpiCmdType & SCPI::isQuery) > 0)) // test if we got an allowed query
    {
        QString answer = m_pModule->m_pStorageSystem->getStoredValue(m_pSCPICmdInfo->entityId, m_pSCPICmdInfo->componentName).toString();
        emit signalAnswer(answer);
        //client->receiveAnswer(answer);
    }

    else

        if (cmd.isCommand(1) && ((scpiCmdType & SCPI::isCmdwP) > 0)) // test if we got an allowed cmd + 1 parameter
        {
            VeinComponent::ComponentData *cData;
            QVariant oldValue = m_pModule->m_pStorageSystem->getStoredValue(m_pSCPICmdInfo->entityId, m_pSCPICmdInfo->componentName);
            QVariant newValue = cmd.getParam(0);
            newValue.convert(oldValue.type());

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

            cSCPIClientInfo *clientinfo = new cSCPIClientInfo(client, m_pSCPICmdInfo->entityId);
            m_pModule->scpiClientInfoHash.insert(m_pSCPICmdInfo->componentName, clientinfo);

            m_pModule->m_pSCPIEventSystem->sigSendEvent(event);
        }

        else
            emit signalStatus(SCPI::nak);
            //client->receiveStatus(SCPI::nak);

    disconnect(myConn);

    return true;
}

}
