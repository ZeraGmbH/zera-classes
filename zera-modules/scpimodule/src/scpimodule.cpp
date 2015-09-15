#include <rminterface.h>
#include <dspinterface.h>
#include <proxy.h>
#include <veinmodulemetadata.h>
#include <veinmoduleactvalue.h>
#include <veinmoduleparameter.h>
#include <scpiinfo.h>
#include <ve_storagesystem.h>
#include <ve_commandevent.h>
#include <vcmp_entitydata.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include "scpimodule.h"
#include "scpimoduleconfiguration.h"
#include "scpimoduleconfigdata.h"
#include "scpiserver.h"
#include "veinmodulecomponent.h"
#include "scpieventsystem.h"


namespace SCPIMODULE
{

cSCPIModule::cSCPIModule(quint8 modnr, Zera::Proxy::cProxy *proxi, int entityId, VeinEvent::StorageSystem* storagesystem, QObject *parent)
    :cBaseModule(modnr, proxi, entityId, storagesystem, new cSCPIModuleConfiguration(), parent)
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(modnr);
    m_sModuleDescription = QString("This module provides a scpi interface depending on the actual session running");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(modnr);

    m_pSCPIEventSystem = new cSCPIEventSystem(this);
}


cSCPIModule::~cSCPIModule()
{
    delete m_pConfiguration;
}


QByteArray cSCPIModule::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
}


void cSCPIModule::doConfiguration(QByteArray xmlConfigData)
{
    m_pConfiguration->setConfiguration(xmlConfigData);
}


void cSCPIModule::setupModule()
{
    emit addEventSystem(m_pSCPIEventSystem);

    VeinComponent::EntityData *eData = new VeinComponent::EntityData();
    eData->setCommand(VeinComponent::EntityData::Command::ECMD_ADD);
    eData->setEntityId(m_nEntityId);
    VeinEvent::CommandEvent *tmpEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::TRANSACTION, eData);
    m_pSCPIEventSystem->sigSendEvent(tmpEvent);

    VeinComponent::ComponentData *cData;

    cData = new VeinComponent::ComponentData();
    cData->setEntityId(m_nEntityId);
    cData->setCommand(VeinComponent::ComponentData::Command::CCMD_ADD);
    cData->setComponentName("EntityName");
    cData->setNewValue(m_sModuleName);
    tmpEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, cData);
    m_pSCPIEventSystem->sigSendEvent(tmpEvent);

    cSCPIModuleConfigData *pConfData;
    pConfData = qobject_cast<cSCPIModuleConfiguration*>(m_pConfiguration)->getConfigurationData();

    m_pModuleErrorComponent = new cVeinModuleComponent(m_nEntityId, m_pSCPIEventSystem,
                                                       QString("ERR_Message"),
                                                       QString("Component forwards the run time errors"),
                                                       QVariant(QString("")));
    veinModuleComponentList.append(m_pModuleErrorComponent);
    m_pModuleInterfaceComponent = new cVeinModuleComponent(m_nEntityId, m_pSCPIEventSystem,
                                                           QString("INF_ModuleInterface"),
                                                           QString("Component forwards the modules interface"),
                                                           QByteArray());
    veinModuleComponentList.append(m_pModuleInterfaceComponent);

    m_pModuleDescription = new cVeinModuleMetaData(QString("Description"), QVariant(m_sModuleDescription));
    veinModuleMetaDataList.append(m_pModuleDescription);

    // we only have this activist
    m_pSCPIServer = new cSCPIServer(this, *pConfData);
    m_ModuleActivistList.append(m_pSCPIServer);
    connect(m_pSCPIServer, SIGNAL(activated()), SIGNAL(activationContinue()));
    connect(m_pSCPIServer, SIGNAL(deactivated()), this, SIGNAL(deactivationContinue()));
    connect(m_pSCPIServer, SIGNAL(errMsg(QVariant)), m_pModuleErrorComponent, SLOT(setValue(QVariant)));

    // we already post meta information here because setting up interface looks for valid meta info
    exportMetaData();

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}


void cSCPIModule::unsetModule()
{
    if (m_ModuleActivistList.count() > 0)
    {
        for (int i = 0; i < m_ModuleActivistList.count(); i++)
        {
            m_ModuleActivistList.at(i)->deleteInterface();
            delete m_ModuleActivistList.at(i);
        }

        m_ModuleActivistList.clear();
    }
}


void cSCPIModule::startMeas()
{
    // we don't start the interface expl.
}


void cSCPIModule::stopMeas()
{
    // we don't stop the interface expl.
}


void cSCPIModule::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}


void cSCPIModule::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}


void cSCPIModule::activationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}


void cSCPIModule::activationFinished()
{
    emit addEventSystem(m_pSCPIEventSystem);
    emit activationReady();
}


void cSCPIModule::deactivationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}


void cSCPIModule::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}


void cSCPIModule::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}


void cSCPIModule::deactivationFinished()
{
    emit deactivationReady();
}

}
