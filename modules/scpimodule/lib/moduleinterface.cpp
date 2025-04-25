#include <QList>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

#include <vs_abstracteventsystem.h>
#include <ve_commandevent.h>
#include <vcmp_componentdata.h>

#include <scpi.h>

#include "scpimodule.h"
#include "scpimeasure.h"
#include "scpiinterface.h"
#include "moduleinterface.h"
#include "scpicmdinfo.h"
#include "scpiparameterdelegate.h"
#include "scpicatalogcmddelegate.h"
#include "scpimeasuredelegate.h"


namespace SCPIMODULE
{

cModuleInterface::cModuleInterface(cSCPIModule* module, cSCPIInterface *iface)
    :cBaseInterface(module, iface)
{
}

cModuleInterface::~cModuleInterface()
{
    while(!m_scpiDelegateList.isEmpty())
        delete m_scpiDelegateList.takeLast();
    for(const auto &delegate : qAsConst(m_scpiMeasureDelegateHash))
        delete delegate;
    m_scpiMeasureDelegateHash.clear();
    for (auto measureObject: m_measureObjectsToDelete)
        delete measureObject;
    for (auto scpiCmdInfo : m_scpiCmdInfosToDelete)
        delete scpiCmdInfo;
}

bool cModuleInterface::setupInterface()
{
    bool ok = true;
    const VeinStorage::AbstractDatabase* storageDb = m_pModule->getStorageDb();
    const QList<int> entityIdList = storageDb->getEntityList();
    for(auto entityID : entityIdList) {
        // we parse over all moduleinterface components
        if (storageDb->hasStoredValue(entityID, QString("INF_ModuleInterface"))) {
            QJsonDocument jsonDoc = QJsonDocument::fromJson(storageDb->getStoredValue(entityID, QString("INF_ModuleInterface")).toByteArray());
            if ( !jsonDoc.isNull() && jsonDoc.isObject() ) {
                const QJsonObject jsonObj = jsonDoc.object();
                const QJsonObject jsonScpiInfo = jsonObj["SCPIInfo"].toObject();
                const QJsonObject jsonComponentInfo = jsonObj["ComponentInfo"].toObject();
                QString scpiModuleName = jsonScpiInfo["Name"].toString();

                QJsonArray jsonScpiCmdArr = jsonScpiInfo["Cmd"].toArray();
                for (int j = 0; j < jsonScpiCmdArr.count(); j++) {
                    cSCPICmdInfo *scpiCmdInfo = new cSCPICmdInfo();
                    m_scpiCmdInfosToDelete.append(scpiCmdInfo);
                    scpiCmdInfo->scpiModuleName = scpiModuleName;
                    scpiCmdInfo->entityId = entityID;
                    QJsonArray jsonCmdArr = jsonScpiCmdArr[j].toArray();
                    scpiCmdInfo->scpiModel = jsonCmdArr[0].toString();
                    scpiCmdInfo->scpiCommand = jsonCmdArr[1].toString();
                    scpiCmdInfo->scpiCommandType = jsonCmdArr[2].toString();
                    scpiCmdInfo->componentName = jsonCmdArr[3].toString();
                    scpiCmdInfo->veinComponentInfo = jsonComponentInfo[scpiCmdInfo->componentName].toObject();
                    scpiCmdInfo->refType = jsonCmdArr[4].toString();

                    addSCPICommand(scpiCmdInfo); // we add our command now
                }
            }
            else
                ok = false;
        }
    }
    return ok;
}

void cModuleInterface::actualizeInterface(QVariant modInterface)
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
            if (jsonCmdArr[4].toString() != "0") { // so it is a property delegate
                QString cmdComplete;
                cmdComplete = QString("%1:%2:%3").arg(jsonCmdArr[0].toString(), scpiModuleName, jsonCmdArr[1].toString());
                m_scpiPropertyDelegateHash[cmdComplete]->setOutput(modInterface);
            }
        }
    }
}

QHash<QString, cSCPIMeasureDelegate *> *cModuleInterface::getSCPIMeasDelegateHash()
{
    return &m_scpiMeasureDelegateHash;
}

void cModuleInterface::addSCPICommand(cSCPICmdInfo *scpiCmdInfo)
{
    if (scpiCmdInfo->scpiModel == "MEASURE") {
        // in case of measure model we have to add several commands for each value
        cSCPIMeasure* measureObject = new cSCPIMeasure(&m_pModule->scpiMeasureHash, scpiCmdInfo);
        m_measureObjectsToDelete.append(measureObject);

        addSCPIMeasureCommand(QString(""), QString("MEASURE"), SCPI::isNode | SCPI::isQuery, SCPIModelType::measure, measureObject);
        addSCPIMeasureCommand(QString(""), QString("CONFIGURE"), SCPI::isNode | SCPI::isCmd, SCPIModelType::configure, measureObject);
        addSCPIMeasureCommand(QString(""), QString("READ"), SCPI::isNode | SCPI::isQuery, SCPIModelType::read, measureObject);
        addSCPIMeasureCommand(QString(""), QString("INIT"), SCPI::isNode | SCPI::isCmd, SCPIModelType::init, measureObject);
        addSCPIMeasureCommand(QString(""), QString("FETCH"), SCPI::isNode | SCPI::isQuery, SCPIModelType::fetch, measureObject);

        addSCPIMeasureCommand(QString("MEASURE"), scpiCmdInfo->scpiModuleName, SCPI::isNode | SCPI::isQuery, SCPIModelType::measure, measureObject);
        addSCPIMeasureCommand(QString("CONFIGURE"), scpiCmdInfo->scpiModuleName, SCPI::isNode | SCPI::isCmd, SCPIModelType::configure, measureObject);
        addSCPIMeasureCommand(QString("READ"), scpiCmdInfo->scpiModuleName, SCPI::isNode | SCPI::isQuery, SCPIModelType::read, measureObject);
        addSCPIMeasureCommand(QString("INIT"), scpiCmdInfo->scpiModuleName, SCPI::isNode | SCPI::isCmd, SCPIModelType::init, measureObject);
        addSCPIMeasureCommand(QString("FETCH"), scpiCmdInfo->scpiModuleName, SCPI::isNode | SCPI::isQuery, SCPIModelType::fetch, measureObject);

        addSCPIMeasureCommand(QString("MEASURE:%2").arg(scpiCmdInfo->scpiModuleName), scpiCmdInfo->scpiCommand, SCPI::isQuery, SCPIModelType::measure, measureObject, scpiCmdInfo->veinComponentInfo);
        addSCPIMeasureCommand(QString("CONFIGURE:%2").arg(scpiCmdInfo->scpiModuleName), scpiCmdInfo->scpiCommand, SCPI::isCmd, SCPIModelType::configure, measureObject, scpiCmdInfo->veinComponentInfo);
        addSCPIMeasureCommand(QString("READ:%2").arg(scpiCmdInfo->scpiModuleName), scpiCmdInfo->scpiCommand, SCPI::isQuery, SCPIModelType::read, measureObject, scpiCmdInfo->veinComponentInfo);
        addSCPIMeasureCommand(QString("INIT:%2").arg(scpiCmdInfo->scpiModuleName), scpiCmdInfo->scpiCommand, SCPI::isCmd, SCPIModelType::init, measureObject, scpiCmdInfo->veinComponentInfo);
        addSCPIMeasureCommand(QString("FETCH:%2").arg(scpiCmdInfo->scpiModuleName), scpiCmdInfo->scpiCommand, SCPI::isQuery, SCPIModelType::fetch, measureObject, scpiCmdInfo->veinComponentInfo);

    }
    else {
        QString cmdComplete = QString("%1:%2:%3").arg(scpiCmdInfo->scpiModel, scpiCmdInfo->scpiModuleName, scpiCmdInfo->scpiCommand);
        QStringList nodeNames = cmdComplete.split(':');
        QString cmdNode = nodeNames.takeLast();
        QString cmdParent = nodeNames.join(':');
        ScpiBaseDelegate* delegate;
        if (scpiCmdInfo->refType == "0") {
            delegate = new cSCPIParameterDelegate(cmdParent, cmdNode, scpiCmdInfo->scpiCommandType.toInt(), m_pModule, scpiCmdInfo);
            setXmlComponentInfo(delegate, scpiCmdInfo->veinComponentInfo);
        }
        else {
            delegate = new cSCPICatalogCmdDelegate(cmdParent, cmdNode, scpiCmdInfo->scpiCommandType.toInt(), m_pModule, scpiCmdInfo);
            m_scpiPropertyDelegateHash[cmdComplete] = static_cast<cSCPICatalogCmdDelegate*>(delegate); // for easier access if we need to change answers of this delegate
        }
        m_scpiDelegateList.append(delegate); // for clean up .....
        m_pSCPIInterface->addSCPICommand(delegate);
    }
}


void cModuleInterface::addSCPIMeasureCommand(QString cmdparent, QString cmd, quint8 cmdType, quint8 measCode, cSCPIMeasure *measureObject, QJsonObject veinComponentInfo)
{
    QString cmdcomplete = QString("%1:%2").arg(cmdparent, cmd);
    cSCPIMeasureDelegate* delegate;
    if (m_scpiMeasureDelegateHash.contains(cmdcomplete)) {
        delegate = m_scpiMeasureDelegateHash.value(cmdcomplete);
        delegate->addscpimeasureObject(measureObject);
    }
    else {
        delegate = new cSCPIMeasureDelegate(cmdparent, cmd, cmdType , measCode, measureObject);
        m_scpiMeasureDelegateHash[cmdcomplete] = delegate;
        m_pSCPIInterface->addSCPICommand(delegate);
    }
    setXmlComponentInfo(delegate, veinComponentInfo);
}

void cModuleInterface::setXmlComponentInfo(ScpiBaseDelegate *delegate, const QJsonObject &componentInfo)
{
    QString desc = componentInfo["Description"].toString();
    if(!desc.isEmpty())
        delegate->setXmlAttribute("Description", desc);
    setXmlComponentValidatorInfo(delegate, componentInfo);
    QString unit = componentInfo["Unit"].toString();
    if(!unit.isEmpty())
        delegate->setXmlAttribute("Unit", unit);
}

void cModuleInterface::setXmlComponentValidatorInfo(ScpiBaseDelegate *delegate, const QJsonObject &componentInfo)
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

QJsonArray cModuleInterface::getValidatorEntries(QJsonObject validator)
{
    return validator["Data"].toArray();
}

}
