#include "scpimodelmeasureandfriends.h"
#include "scpimodule.h"
#include "scpidelegateparameter.h"
#include "scpidelegatecatalog.h"
#include "scpidelegaterpc.h"
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
    const VeinScpiModuleInterfaceParser::ScpiParseInfo allRpcInfos = moduleInterfaces.getRpcInfo();

    const VeinStorage::AbstractDatabase* storageDb = m_pModule->getStorageDb();
    const QList<int> entityIdList = storageDb->getEntityList();
    for(auto entityID : entityIdList) {
        if (allComponentInfos.contains(entityID)) {
            const QList<cSCPICmdInfoPtr> &commands = allComponentInfos[entityID];
            for (const cSCPICmdInfoPtr &command : commands)
                addSCPICommand(scpiInterface, command);
        }
        if (allRpcInfos.contains(entityID)) {
            const QList<cSCPICmdInfoPtr> &rpcs = allRpcInfos[entityID];
            for (const cSCPICmdInfoPtr &rpc : rpcs)
                addRPCCommand(scpiInterface, rpc);
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
        if (scpiCmdInfo->refType == "0") {
            delegate = std::make_shared<ScpiDelegateParameter>(ScpiDelegateParameter::Params{cmdParent, cmdNode, scpiCmdInfo->scpiCmdQueryFlagsAsString.toInt(), m_pModule, scpiCmdInfo});
            setXmlComponentInfo(delegate, scpiCmdInfo->veinComponentInfo);
        }
        else {
            delegate = std::make_shared<ScpiDelegateCatalog>(ScpiDelegateCatalog::Params{cmdParent, cmdNode, scpiCmdInfo->scpiCmdQueryFlagsAsString.toInt(), m_pModule, scpiCmdInfo});
            m_scpiCatalogDelegateHash[scpiPath] = static_cast<ScpiDelegateCatalog*>(delegate.get()); // for easier access if we need to change answers of this delegate
        }
        scpiInterface->addSCPICommand(delegate);
    }
}

void ScpiModelMeasureAndFriends::addRPCCommand(cSCPIInterface *scpiInterface, const cSCPICmdInfoPtr &scpiCmdInfo)
{
    QString cmdComplete = QString("%1:%2:%3").arg(scpiCmdInfo->scpiModel, scpiCmdInfo->scpiModuleName, scpiCmdInfo->scpiCommand);
    QStringList nodeNames = cmdComplete.split(':');
    QString cmdNode = nodeNames.takeLast();
    QString cmdParent = nodeNames.join(':');
    ScpiBaseDelegatePtr delegate = std::make_shared<ScpiDelegateRpc>(ScpiDelegateRpc::Params{cmdParent, cmdNode, scpiCmdInfo->scpiCmdQueryFlagsAsString.toInt(), m_pModule, scpiCmdInfo});
    setXmlComponentInfo(delegate, scpiCmdInfo->veinComponentInfo);
    scpiInterface->addSCPICommand(delegate);
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
    setXmlComponentInfo(delegate, veinComponentInfo);
}

void ScpiModelMeasureAndFriends::setXmlComponentInfo(ScpiBaseDelegatePtr delegate, const QJsonObject &componentInfo)
{
    QString desc = componentInfo["Description"].toString();
    if(!desc.isEmpty())
        delegate->setXmlAttribute("Description", desc);
    setXmlComponentValidatorInfo(delegate, componentInfo);
    QString unit = componentInfo["Unit"].toString();
    if(!unit.isEmpty())
        delegate->setXmlAttribute("Unit", unit);
}

void ScpiModelMeasureAndFriends::setXmlComponentValidatorInfo(ScpiBaseDelegatePtr delegate, const QJsonObject &componentInfo)
{
    QJsonObject validator = componentInfo["Validation"].toObject();
    QString validatorType = validator["Type"].toString();
    if(!validatorType.isEmpty()) {
        if(validatorType == "STRING") {
            delegate->setXmlAttribute("DataType", validatorType);
            QJsonArray validatorEntryArray = getValidatorEntries(validator);
            QStringList validStrings;
            for(const auto &entry : qAsConst(validatorEntryArray)) {
                validStrings.append(entry.toString());
            }
            if(!validStrings.isEmpty())
                delegate->setXmlAttribute("ValidPar", validStrings.join(","));
        }
        else if(validatorType == "BOOL") {
            delegate->setXmlAttribute("DataType", validatorType);
            delegate->setXmlAttribute("Min", QString("0"));
            delegate->setXmlAttribute("Max", QString("1"));
        }
        else if(validatorType == "INTEGER" || validatorType == "DOUBLE") {
            delegate->setXmlAttribute("DataType", validatorType);
            QJsonArray validatorEntryArray = getValidatorEntries(validator);
            QString minVal = validatorEntryArray[0].toVariant().toString();
            QString maxVal = validatorEntryArray[1].toVariant().toString();
            if(!minVal.isEmpty() && !maxVal.isEmpty()) {
                delegate->setXmlAttribute("Min", minVal);
                delegate->setXmlAttribute("Max", maxVal);
            }
        }
        else if(validatorType == "REGEX") {
            delegate->setXmlAttribute("DataType", "STRING");
            QString regEx = validator["Data"].toString();
            delegate->setXmlAttribute("RegEx", regEx);
        }
    }
}

QJsonArray ScpiModelMeasureAndFriends::getValidatorEntries(QJsonObject validator)
{
    return validator["Data"].toArray();
}

}
