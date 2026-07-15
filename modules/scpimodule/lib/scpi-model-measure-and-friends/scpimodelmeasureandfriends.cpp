#include "scpimodelmeasureandfriends.h"
#include "scpimodule.h"
#include "scpidelegateparameter.h"
#include "scpidelegatecatalog.h"
#include "scpidelegatexmlexportgenerator.h"
#include <vs_abstracteventsystem.h>
#include <ve_commandevent.h>
#include <vcmp_componentdata.h>
#include <scpi.h>
#include <QJsonDocument>
#include <QJsonValue>

namespace SCPIMODULE
{

ScpiModelMeasureAndFriends::ScpiModelMeasureAndFriends(cSCPIModule* module) :
    m_pModule(module)
{
}

ScpiModelMeasureAndFriends::~ScpiModelMeasureAndFriends()
{
    m_scpiMeasureDelegateHash.clear();
    for (auto measureObject: m_measureObjectsToDelete)
        delete measureObject;
}

bool ScpiModelMeasureAndFriends::setupScpi(cSCPIInterface *scpiInterface)
{
    const VeinScpiModuleInterfaceParser moduleInterfaces = m_pModule->getScpiModuleInterfaceParser();
    const VeinScpiModuleInterfaceParser::ScpiParseInfo allComponentInfos = moduleInterfaces.getComponentInfo();
    const VeinStorage::AbstractDatabase* storageDb = m_pModule->getStorageDb();

    const QList<int> entityIdList = storageDb->getEntityList();
    for(auto entityID : entityIdList) {
        if (allComponentInfos.contains(entityID)) {
            const QList<cSCPICmdInfoPtr> &commands = allComponentInfos[entityID];
            for (const cSCPICmdInfoPtr &command : commands)
                addSCPICommand(scpiInterface, command);
        }
    }
    return true;
}

void ScpiModelMeasureAndFriends::actualizeCatalogs(const QVariant &modInterface)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(modInterface.toByteArray());
    if ( !jsonDoc.isNull() && jsonDoc.isObject() ) {
        QJsonObject jsonObj = jsonDoc.object();
        jsonObj = jsonObj["SCPIInfo"].toObject();
        QString scpiModuleName = jsonObj["Name"].toString();
        QJsonArray jsonArr = jsonObj["Cmd"].toArray();
        // we iterate over all cmds
        for (int j = 0; j < jsonArr.count(); j++) {
            QJsonArray jsonCmdArr = jsonArr[j].toArray();
            if (jsonCmdArr[4].toString() != "0") { // so it is a catalog delegate
                QString scpiPath = QString("%1:%2:%3").arg(jsonCmdArr[0].toString(), scpiModuleName, jsonCmdArr[1].toString());
                m_scpiCatalogDelegateHash[scpiPath]->setOutputFromInfModuleInterface(modInterface);
            }
        }
    }
}

QHash<QString, MeasureScpiNodeDelegatePtr> *ScpiModelMeasureAndFriends::getSCPIMeasDelegateHash()
{
    return &m_scpiMeasureDelegateHash;
}

void ScpiModelMeasureAndFriends::updatePendingMeasureSequences(int entityId, const QString &componentName, const QVariant &newValue)
{
    const QList<VeinComponentScpiMeasureSequence*> scpiMeasureList = VeinComponentScpiMeasureSequence::getVeinComponentScpiMeasureSequenceStoreSingleton()->values(componentName);
    for(int i = 0; i < scpiMeasureList.count(); i++) {
        VeinComponentScpiMeasureSequence *scpiMeasure = scpiMeasureList.at(i);
        if(scpiMeasure->entityID() == entityId)
            scpiMeasure->receiveMeasureValue(newValue);
    }
}

void ScpiModelMeasureAndFriends::addSCPICommand(cSCPIInterface *scpiInterface, const cSCPICmdInfoPtr &scpiCmdInfo)
{
    if (scpiCmdInfo->scpiModel == "MEASURE") {
        // in case of measure model we have to add several commands for each value
        VeinComponentScpiMeasureSequence* measureObject = new VeinComponentScpiMeasureSequence(scpiCmdInfo);
        m_measureObjectsToDelete.append(measureObject);

        addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{"", "MEASURE", SCPI::isNode | SCPI::isQuery, ScpiModelTypes::measure, measureObject});
        addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{"", "CONFIGURE", SCPI::isNode | SCPI::isCmd, ScpiModelTypes::configure, measureObject});
        addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{"", "READ", SCPI::isNode | SCPI::isQuery, ScpiModelTypes::read, measureObject});
        addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{"", "INIT", SCPI::isNode | SCPI::isCmd, ScpiModelTypes::init, measureObject});
        addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{"", "FETCH", SCPI::isNode | SCPI::isQuery, ScpiModelTypes::fetch, measureObject});

        addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{"MEASURE", scpiCmdInfo->scpiModuleName, SCPI::isNode | SCPI::isQuery, ScpiModelTypes::measure, measureObject});
        addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{"CONFIGURE", scpiCmdInfo->scpiModuleName, SCPI::isNode | SCPI::isCmd, ScpiModelTypes::configure, measureObject});
        addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{"READ", scpiCmdInfo->scpiModuleName, SCPI::isNode | SCPI::isQuery, ScpiModelTypes::read, measureObject});
        addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{"INIT", scpiCmdInfo->scpiModuleName, SCPI::isNode | SCPI::isCmd, ScpiModelTypes::init, measureObject});
        addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{"FETCH", scpiCmdInfo->scpiModuleName, SCPI::isNode | SCPI::isQuery, ScpiModelTypes::fetch, measureObject});

        addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{QString("MEASURE:%2").arg(scpiCmdInfo->scpiModuleName), scpiCmdInfo->scpiCommand, SCPI::isQuery, ScpiModelTypes::measure, measureObject},
                              scpiCmdInfo->veinComponentInfo);
        addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{QString("CONFIGURE:%2").arg(scpiCmdInfo->scpiModuleName), scpiCmdInfo->scpiCommand, SCPI::isCmd, ScpiModelTypes::configure, measureObject},
                              scpiCmdInfo->veinComponentInfo);
        addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{QString("READ:%2").arg(scpiCmdInfo->scpiModuleName), scpiCmdInfo->scpiCommand, SCPI::isQuery, ScpiModelTypes::read, measureObject},
                              scpiCmdInfo->veinComponentInfo);
        addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{QString("INIT:%2").arg(scpiCmdInfo->scpiModuleName), scpiCmdInfo->scpiCommand, SCPI::isCmd, ScpiModelTypes::init, measureObject},
                              scpiCmdInfo->veinComponentInfo);
        addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{QString("FETCH:%2").arg(scpiCmdInfo->scpiModuleName), scpiCmdInfo->scpiCommand, SCPI::isQuery, ScpiModelTypes::fetch, measureObject},
                              scpiCmdInfo->veinComponentInfo);

    }
    else {
        QString scpiPath = QString("%1:%2:%3").arg(scpiCmdInfo->scpiModel, scpiCmdInfo->scpiModuleName, scpiCmdInfo->scpiCommand);
        QStringList nodeNames = scpiPath.split(':');
        QString cmdNode = nodeNames.takeLast();
        QString cmdParent = nodeNames.join(':');
        ScpiBaseDelegatePtr delegate;
        if (scpiCmdInfo->refType != "0") {
            delegate = std::make_shared<ScpiDelegateCatalog>(ScpiDelegateCatalog::Params{cmdParent, cmdNode, scpiCmdInfo->scpiCmdQueryFlags, m_pModule, scpiCmdInfo});
            m_scpiCatalogDelegateHash[scpiPath] = static_cast<ScpiDelegateCatalog*>(delegate.get()); // for easier access if we need to change answers of this delegate
            scpiInterface->addSCPICommand(delegate);
        }
    }
}

void ScpiModelMeasureAndFriends::addSCPIMeasureCommand(cSCPIInterface *scpiInterface,
                                                       const ScpiDelegateMeasure::Params &params,
                                                       const QJsonObject &veinComponentInfo)
{
    QString cmdcomplete = QString("%1:%2").arg(params.cmdParent, params.cmd);
    MeasureScpiNodeDelegatePtr delegate;
    if (m_scpiMeasureDelegateHash.contains(cmdcomplete)) {
        delegate = m_scpiMeasureDelegateHash.value(cmdcomplete);
        delegate->addVeinComponentScpiSequence(params.scpiMeasureObject);
    }
    else {
        delegate = std::make_shared<ScpiDelegateMeasure>(params);
        m_scpiMeasureDelegateHash[cmdcomplete] = delegate;
        scpiInterface->addSCPICommand(delegate);
    }
    ScpiDelegateXmlExportGenerator::setXmlComponentInfo(delegate, veinComponentInfo);
}

}
