#include <QList>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

#include <ve_storagesystem.h>
#include <ve_commandevent.h>
#include <vcmp_componentdata.h>

#include <scpi.h>

#include "scpimodule.h"
#include "scpimeasure.h"
#include "scpiinterface.h"
#include "moduleinterface.h"
#include "scpicmdinfo.h"
#include "scpiparameterdelegate.h"
#include "scpipropertydelegate.h"
#include "scpimeasuredelegate.h"


namespace SCPIMODULE
{

cModuleInterface::cModuleInterface(cSCPIModule* module, cSCPIInterface *iface)
    :cBaseInterface(module, iface)
{
}


cModuleInterface::~cModuleInterface()
{
    int n;

    n = m_scpiDelegateList.count();
    if (n > 0)
        for (int i = 0; i < n; i++ )
            delete m_scpiDelegateList.at(i);

    QList<cSCPIMeasureDelegate*> mdList = m_scpiMeasureDelegateHash.values();
    n = mdList.count();
    if (n > 0)
        for (int i = 0; i < n; i++ )
            delete mdList.at(i);
}


bool cModuleInterface::setupInterface()
{
    QList<int> entityIdList;
    int n;
    bool noError;

    noError = true;

    entityIdList = m_pModule->m_pStorageSystem->getEntityList();
    n = entityIdList.count();

    for (int i = 0; i < n; i++)
    {
        int entityID;
        entityID = entityIdList.at(i);
        // we parse over all moduleinterface components
        if (m_pModule->m_pStorageSystem->hasStoredValue(entityID, QString("INF_ModuleInterface")))
        {
            QJsonDocument jsonDoc;
            jsonDoc = QJsonDocument::fromJson(m_pModule->m_pStorageSystem->getStoredValue(entityID, QString("INF_ModuleInterface")).toByteArray());

            if ( !jsonDoc.isNull() && jsonDoc.isObject() ) {
                const QJsonObject jsonObj = jsonDoc.object();
                const QJsonObject jsonScpiInfo = jsonObj["SCPIInfo"].toObject();
                const QJsonObject jsonComponentInfo = jsonObj["ComponentInfo"].toObject();
                QString scpiModuleName = jsonScpiInfo["Name"].toString();

                QJsonArray jsonScpiCmdArr = jsonScpiInfo["Cmd"].toArray();
                QJsonArray jsonCmdArr;
                for (int j = 0; j < jsonScpiCmdArr.count(); j++) {
                    cSCPICmdInfo *scpiCmdInfo;

                    jsonCmdArr = jsonScpiCmdArr[j].toArray();
                    scpiCmdInfo = new cSCPICmdInfo();

                    scpiCmdInfo->scpiModuleName = scpiModuleName;
                    scpiCmdInfo->entityId = entityID;
                    scpiCmdInfo->scpiModel = jsonCmdArr[0].toString();
                    scpiCmdInfo->scpiCommand = jsonCmdArr[1].toString();
                    scpiCmdInfo->scpiCommandType = jsonCmdArr[2].toString();
                    scpiCmdInfo->componentName = jsonCmdArr[3].toString();
                    scpiCmdInfo->veinComponentInfo = jsonComponentInfo[scpiCmdInfo->componentName].toObject();
                    scpiCmdInfo->refType = jsonCmdArr[4].toString();
                    scpiCmdInfo->unit = jsonCmdArr[5].toString();

                    addSCPICommand(scpiCmdInfo); // we add our command now
                }
            }
            else
                noError = false;
        }
    }

    return noError;
}


void cModuleInterface::actualizeInterface(QVariant modInterface)
{
    QJsonDocument jsonDoc;
    jsonDoc = QJsonDocument::fromJson(modInterface.toByteArray());

    if ( !jsonDoc.isNull() && jsonDoc.isObject() )
    {
        QString scpiModuleName;
        QJsonObject jsonObj;
        QJsonArray jsonArr, jsonCmdArr;

        jsonObj = jsonDoc.object();

        jsonObj = jsonObj["SCPIInfo"].toObject();
        scpiModuleName = jsonObj["Name"].toString();

        jsonArr = jsonObj["Cmd"].toArray();

        // we iterate over all cmds
        for (int j = 0; j < jsonArr.count(); j++)
        {
            jsonCmdArr = jsonArr[j].toArray();

            if (jsonCmdArr[4].toString() != "0")
            { // so it is a property delegate

                //cSCPICmdInfo *scpiCmdInfo;
                QString cmdComplete;
                cmdComplete = QString("%1:%2:%3").arg(jsonCmdArr[0].toString(), scpiModuleName, jsonCmdArr[1].toString());
                m_scpiPropertyDelegateHash[cmdComplete]->setOutput(modInterface);
                /*
                scpiCmdInfo = new cSCPICmdInfo();
                scpiCmdInfo->scpiModuleName = scpiModuleName;
                scpiCmdInfo->entityId = entityID;
                scpiCmdInfo->scpiModel = jsonCmdArr[0].toString();
                scpiCmdInfo->scpiCommand = jsonCmdArr[1].toString();
                scpiCmdInfo->scpiCommandType = jsonCmdArr[2].toString();
                scpiCmdInfo->componentName = jsonCmdArr[3].toString();
                scpiCmdInfo->refType = jsonCmdArr[4].toString();
                scpiCmdInfo->unit = jsonCmdArr[5].toString();
                cmdComplete = QString("%1:%2:%3").arg(scpiCmdInfo->scpiModel).arg(scpiCmdInfo->scpiModuleName).arg(scpiCmdInfo->scpiCommand);
                m_scpiPropertyDelegateHash[cmdComplete]->setOutput(scpiCmdInfo);
                */
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
    if (scpiCmdInfo->scpiModel == "MEASURE")

    {
        // in case of measure model we have to add several commands for each value

        cSCPIMeasure* measureObject = new cSCPIMeasure(m_pModule, scpiCmdInfo);

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
    else
    {
        bool ok;
        cSCPIDelegate* delegate;

        QString cmdComplete;
        cmdComplete = QString("%1:%2:%3").arg(scpiCmdInfo->scpiModel, scpiCmdInfo->scpiModuleName, scpiCmdInfo->scpiCommand);
        QStringList nodeNames = cmdComplete.split(':');
        QString cmdNode = nodeNames.takeLast();
        QString cmdParent = nodeNames.join(':');

        if (scpiCmdInfo->refType == "0") {
            delegate = new cSCPIParameterDelegate(cmdParent, cmdNode, scpiCmdInfo->scpiCommandType.toInt(&ok), m_pModule, scpiCmdInfo);
            setXmlComponentInfo(delegate, scpiCmdInfo->veinComponentInfo);
        }
        else
        {
            delegate = new cSCPIPropertyDelegate(cmdParent, cmdNode, scpiCmdInfo->scpiCommandType.toInt(&ok), m_pModule, scpiCmdInfo);
            m_scpiPropertyDelegateHash[cmdComplete] = static_cast<cSCPIPropertyDelegate*>(delegate); // for easier access if we need to change answers of this delegate
        }

        m_scpiDelegateList.append(delegate); // for clean up .....
        m_pSCPIInterface->addSCPICommand(delegate);
    }
}


void cModuleInterface::addSCPIMeasureCommand(QString cmdparent, QString cmd, quint8 cmdType, quint8 measCode, cSCPIMeasure *measureObject, QJsonObject veinComponentInfo)
{
    cSCPIMeasureDelegate* delegate;
    QString cmdcomplete = QString("%1:%2").arg(cmdparent, cmd);

    if (m_scpiMeasureDelegateHash.contains(cmdcomplete))
    {
        delegate = m_scpiMeasureDelegateHash.value(cmdcomplete);
        delegate->addscpimeasureObject(measureObject);
    }
    else
    {
        delegate = new cSCPIMeasureDelegate(cmdparent, cmd, cmdType , measCode, measureObject);
        m_scpiMeasureDelegateHash[cmdcomplete] = delegate;
        m_pSCPIInterface->addSCPICommand(delegate);
    }
    setXmlComponentInfo(delegate, veinComponentInfo);
}

void cModuleInterface::setXmlComponentInfo(cSCPIDelegate *delegate, const QJsonObject &componentInfo)
{
    QString desc = componentInfo["Description"].toString();
    if(!desc.isEmpty())
        delegate->setXmlAttribute("Description", desc);
    QJsonObject validator = componentInfo["Validation"].toObject();
    QString validatorType = validator["Type"].toString();
    if(!validatorType.isEmpty()) {
        delegate->setXmlAttribute("DataType", validatorType);
        if(validatorType == "STRING") {
            QJsonArray validatorEntryArray = getValidatorEntries(validator);
            QStringList validStrings;
            for(const auto &entry : qAsConst(validatorEntryArray)) {
                validStrings.append(entry.toString());
            }
            if(!validStrings.isEmpty()) {
                delegate->setXmlAttribute("ValidPar", validStrings.join(","));
            }
        }
        if(validatorType == "INTEGER" || validatorType == "DOUBLE") {
            QJsonArray validatorEntryArray = getValidatorEntries(validator);
            QString minVal = validatorEntryArray[0].toVariant().toString();
            QString maxVal = validatorEntryArray[1].toVariant().toString();
            if(!minVal.isEmpty() && !maxVal.isEmpty()) {
                delegate->setXmlAttribute("Min", minVal);
                delegate->setXmlAttribute("Max", maxVal);
            }
        }
    }
}

QJsonArray cModuleInterface::getValidatorEntries(QJsonObject validator)
{
    return validator["Data"].toArray();
}

}
