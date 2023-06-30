#include "customerdatasystem.h"

#include <ve_eventdata.h>
#include <ve_commandevent.h>
#include <ve_storagesystem.h>
#include <vcmp_componentdata.h>
#include <vcmp_entitydata.h>
#include <vcmp_errordata.h>
#include <vcmp_introspectiondata.h>
#include <vcmp_remoteproceduredata.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QSaveFile>
#include <QFile>
#include <QtConcurrent>

#include <functional>

using namespace VeinComponent;

const QHash<QString, QString> CustomerDataSystem::s_componentIntrospection = {
    {CustomerDataSystem::s_entityNameComponentName, CustomerDataSystem::s_entityNameComponentDescription},
    {CustomerDataSystem::s_fileSelectedComponentName, CustomerDataSystem::s_fileSelectedComponentDescription},
    //base
    {CustomerDataSystem::s_baseIdentifierComponentName, CustomerDataSystem::s_baseIdentifierComponentDescription},
    {CustomerDataSystem::s_baseCommentComponentName, CustomerDataSystem::s_baseCommentComponentDescription},
    //customer
    {CustomerDataSystem::s_customerFirstNameComponentName, CustomerDataSystem::s_customerFirstNameComponentDescription},
    {CustomerDataSystem::s_customerLastNameComponentName, CustomerDataSystem::s_customerLastNameComponentDescription},
    {CustomerDataSystem::s_customerStreetComponentName, CustomerDataSystem::s_customerStreetComponentDescription},
    {CustomerDataSystem::s_customerPostalCodeComponentName, CustomerDataSystem::s_customerPostalCodeComponentDescription},
    {CustomerDataSystem::s_customerCityComponentName, CustomerDataSystem::s_customerCityComponentDescription},
    {CustomerDataSystem::s_customerCountryComponentName, CustomerDataSystem::s_customerCountryComponentDescription},
    {CustomerDataSystem::s_customerNumberComponentName, CustomerDataSystem::s_customerNumberComponentDescription},
    {CustomerDataSystem::s_customerCommentComponentName, CustomerDataSystem::s_customerCommentComponentDescription},
    //location
    {CustomerDataSystem::s_locationFirstNameComponentName, CustomerDataSystem::s_locationFirstNameComponentDescription},
    {CustomerDataSystem::s_locationLastNameComponentName, CustomerDataSystem::s_locationLastNameComponentDescription},
    {CustomerDataSystem::s_locationStreetComponentName, CustomerDataSystem::s_locationStreetComponentDescription},
    {CustomerDataSystem::s_locationPostalCodeComponentName, CustomerDataSystem::s_locationPostalCodeComponentDescription},
    {CustomerDataSystem::s_locationCityComponentName, CustomerDataSystem::s_locationCityComponentDescription},
    {CustomerDataSystem::s_locationCountryComponentName, CustomerDataSystem::s_locationCountryComponentDescription},
    {CustomerDataSystem::s_locationNumberComponentName, CustomerDataSystem::s_locationNumberComponentDescription},
    {CustomerDataSystem::s_locationCommentComponentName, CustomerDataSystem::s_locationCommentComponentDescription},
    //powerGrid
    {CustomerDataSystem::s_powerGridOperatorComponentName, CustomerDataSystem::s_powerGridOperatorComponentDescription},
    {CustomerDataSystem::s_powerGridSupplierComponentName, CustomerDataSystem::s_powerGridSupplierComponentDescription},
    {CustomerDataSystem::s_powerGridCommentComponentName, CustomerDataSystem::s_powerGridCommentComponentDescription},
    //meter
    {CustomerDataSystem::s_meterManufacturerComponentName, CustomerDataSystem::s_meterManufacturerComponentDescription},
    {CustomerDataSystem::s_meterFactoryNumberComponentName, CustomerDataSystem::s_meterFactoryNumberComponentDescription},
    {CustomerDataSystem::s_meterOwnerComponentName, CustomerDataSystem::s_meterOwnerComponentDescription},
    {CustomerDataSystem::s_meterCommentComponentName, CustomerDataSystem::s_meterCommentComponentDescription}
};

const QHash<QString, QString> CustomerDataSystem::s_remoteProcedureIntrospection = {
    {CustomerDataSystem::s_customerDataAddProcedureName, CustomerDataSystem::s_customerDataAddProcedureDescription},
    {CustomerDataSystem::s_customerDataRemoveProcedureName, CustomerDataSystem::s_customerDataRemoveProcedureDescription},
    {CustomerDataSystem::s_customerDataSearchProcedureName, CustomerDataSystem::s_customerDataSearchProcedureDescription}
};

const QSet<QString> CustomerDataSystem::s_writeProtectedComponents = {
    CustomerDataSystem::s_entityNameComponentName,
    CustomerDataSystem::s_introspectionComponentName,
};

CustomerDataSystem::CustomerDataSystem(QObject *t_parent) :
    VeinEvent::EventSystem(t_parent),
    m_remoteProcedures{VF_RPC_BIND(customerDataAdd, std::bind(&CustomerDataSystem::customerDataAdd, this, std::placeholders::_1, std::placeholders::_2)),
                       VF_RPC_BIND(customerDataRemove, std::bind(&CustomerDataSystem::customerDataRemove, this, std::placeholders::_1, std::placeholders::_2)),
                       VF_RPC_BIND(customerDataSearch, std::bind(&CustomerDataSystem::customerDataSearch, this, std::placeholders::_1, std::placeholders::_2))}
{
    Q_ASSERT(QString(MODMAN_CUSTOMERDATA_PATH).isEmpty() == false);
    QFileInfo cDataFileInfo(MODMAN_CUSTOMERDATA_PATH);
    if(cDataFileInfo.exists() && cDataFileInfo.isDir() == false) {
        qWarning() << "Invalid path to customer data, file is not a directory:" << MODMAN_CUSTOMERDATA_PATH;
    }
    else if(cDataFileInfo.exists() == false) {
        QDir customerdataDir;
        qWarning() << "Customer data directory does not exist:" << MODMAN_CUSTOMERDATA_PATH;
        if(customerdataDir.mkdir(MODMAN_CUSTOMERDATA_PATH) == false) {
            qWarning() << "Could not create customerdata direcory:" << MODMAN_CUSTOMERDATA_PATH;
        }
    }

    m_dataWriteDelay.setInterval(50);
    m_dataWriteDelay.setSingleShot(true);
    connect(&m_dataWriteDelay, &QTimer::timeout, this, &CustomerDataSystem::writeCustomerdata);

    connect(this, &CustomerDataSystem::sigDataValueChanged, this, &CustomerDataSystem::updateDataFile);
}

bool CustomerDataSystem::processEvent(QEvent *t_event)
{
    bool retVal = false;
    if(t_event->type() == VeinEvent::CommandEvent::eventType()) {
        VeinEvent::CommandEvent *cEvent = nullptr;
        cEvent = static_cast<VeinEvent::CommandEvent *>(t_event);
        Q_ASSERT(cEvent != nullptr);
        if(cEvent->eventSubtype() != VeinEvent::CommandEvent::EventSubtype::NOTIFICATION //we do not need to process notifications
                && cEvent->eventData()->entityId() == CustomerDataSystem::s_entityId) { //only our own entity is relevant
            switch(cEvent->eventData()->type()) {
            case VeinComponent::ComponentData::dataType(): {
                VeinComponent::ComponentData *cData=nullptr;
                cData = static_cast<VeinComponent::ComponentData *>(cEvent->eventData());
                Q_ASSERT(cData != nullptr);
                if(cData->eventCommand() == VeinComponent::ComponentData::Command::CCMD_SET) {
                    bool validated = false;
                    if(cData->componentName() == CustomerDataSystem::s_fileSelectedComponentName) {
                        if(m_dataWriteDelay.isActive()) { //when a delayed write access for the current dataset is active and the selected file is changed then the write needs to happen before changing
                            m_dataWriteDelay.stop();
                            writeCustomerdata();
                        }
                        //prevent cases where different files exist with the same uppercase and lowercase name, as windows treats them as the same file (if they ever get copied to a windows host)
                        const QString fileName = cData->newValue().toString().toLower();
                        if(!fileName.isEmpty()) {
                            bool jsonExists = QFile(QString("%1%2").arg(MODMAN_CUSTOMERDATA_PATH).arg(fileName)).exists();
                            if(jsonExists) {
                                if(parseCustomerDataFile(fileName) == true) {
                                    validated = true;
                                }
                            }
                            else { //unknown or empty file
                                qWarning() << "Invalid customerdata file selected:" << fileName;
                                retVal = true;
                                VeinComponent::ErrorData *eData = new VeinComponent::ErrorData();
                                eData->setEntityId(CustomerDataSystem::s_entityId);
                                eData->setErrorDescription(QString("Customer data file does not exist: %1").arg(fileName));
                                eData->setOriginalData(cData);
                                eData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
                                eData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
                                VeinEvent::CommandEvent *errorEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, eData);
                                errorEvent->setPeerId(cEvent->peerId());
                                cEvent->accept();
                                emit sigSendEvent(errorEvent);
                            }
                        } else {
                            unloadFile();
                        }
                    }
                    else if(CustomerDataSystem::s_componentIntrospection.contains(cData->componentName()) //validate all data components, except the write protected
                            && CustomerDataSystem::s_writeProtectedComponents.contains(cData->componentName()) == false) {
                        emit sigDataValueChanged(cData->componentName(), cData->newValue().toString());
                        validated = true;
                    }

                    if(validated == true) {
                        retVal = true;
                        VeinEvent::CommandEvent *newEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, new VeinComponent::ComponentData(*cData));
                        newEvent->setPeerId(cEvent->peerId());
                        newEvent->eventData()->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL); //the validated answer is authored from the system that runs the validator (aka. this system)
                        newEvent->eventData()->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL); //inform all users (may or may not result in network messages)
                        cEvent->accept();
                        emit sigSendEvent(newEvent);
                    }
                }
                break;
            }
            case VeinComponent::RemoteProcedureData::dataType(): {
                VeinComponent::RemoteProcedureData *rpcData=nullptr;
                rpcData = static_cast<VeinComponent::RemoteProcedureData *>(cEvent->eventData());
                Q_ASSERT(rpcData != nullptr);
                if(rpcData->command() == VeinComponent::RemoteProcedureData::Command::RPCMD_CALL) {
                    if(m_remoteProcedures.contains(rpcData->procedureName())) {
                        retVal = true;
                        const QUuid callId = rpcData->invokationData().value(VeinComponent::RemoteProcedureData::s_callIdString).toUuid();
                        Q_ASSERT(callId.isNull() == false);
                        Q_ASSERT(m_pendingRpcHash.contains(callId) == false);
                        m_pendingRpcHash.insert(callId, cEvent->peerId());
                        m_remoteProcedures.value(rpcData->procedureName())(callId, rpcData->invokationData());
                        t_event->accept();
                    }
                    else { //unknown procedure
                        retVal = true;
                        qWarning() << "No remote procedure with entityId:" << CustomerDataSystem::s_entityId << "name:" << rpcData->procedureName();
                        VF_ASSERT(false, QStringC(QString("No remote procedure with entityId: %1 name: %2").arg(CustomerDataSystem::s_entityId).arg(rpcData->procedureName())));
                        VeinComponent::ErrorData *eData = new VeinComponent::ErrorData();
                        eData->setEntityId(CustomerDataSystem::s_entityId);
                        eData->setErrorDescription(QString("No remote procedure with name: %1").arg(rpcData->procedureName()));
                        eData->setOriginalData(rpcData);
                        eData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
                        eData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
                        VeinEvent::CommandEvent *errorEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, eData);
                        errorEvent->setPeerId(cEvent->peerId());
                        cEvent->accept();
                        emit sigSendEvent(errorEvent);
                    }
                }
                break;
            }
            } // end switch
        }
    }
    return retVal;
}

void CustomerDataSystem::initializeEntity()
{
    QJsonDocument introspectionDoc;
    QJsonObject introspectionRootObject;
    QJsonObject componentInfo;
    QJsonObject remoteProcedureInfo;

    VeinComponent::EntityData *entityData = new VeinComponent::EntityData();
    entityData->setCommand(VeinComponent::EntityData::Command::ECMD_ADD);
    entityData->setEventOrigin(VeinComponent::EntityData::EventOrigin::EO_LOCAL);
    entityData->setEventTarget(VeinComponent::EntityData::EventTarget::ET_ALL);
    entityData->setEntityId(CustomerDataSystem::s_entityId);
    emit sigSendEvent(new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, entityData));
    entityData = nullptr;

    VeinComponent::ComponentData *initData=nullptr;
    VeinComponent::RemoteProcedureData *rpcData=nullptr;

    const QList<QString> tmpComponentList = CustomerDataSystem::s_componentIntrospection.keys();
    for(const QString &tmpComponentName : qAsConst(tmpComponentList)) {
        QJsonObject tmpComponentObject;
        tmpComponentObject.insert("Description", CustomerDataSystem::s_componentIntrospection.value(tmpComponentName));
        componentInfo.insert(tmpComponentName, tmpComponentObject);

        initData = new VeinComponent::ComponentData();
        initData->setEntityId(CustomerDataSystem::s_entityId);
        initData->setCommand(VeinComponent::ComponentData::Command::CCMD_ADD);
        initData->setComponentName(tmpComponentName);
        initData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
        initData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
        if(tmpComponentName == CustomerDataSystem::s_entityNameComponentName) {
            initData->setNewValue(CustomerDataSystem::s_entityName);
        }
        else {
            initData->setNewValue(QString(""));
        }

        emit sigSendEvent(new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, initData));
        initData = nullptr;
    }
    const QList<QString> tmpRemoteProcedureList = m_remoteProcedures.keys();
    for(const QString &tmpRemoteProcedureName : qAsConst(tmpRemoteProcedureList)) {
        QJsonObject tmpRemoteProcedureInfo;
        Q_ASSERT(CustomerDataSystem::s_remoteProcedureIntrospection.contains(tmpRemoteProcedureName));
        tmpRemoteProcedureInfo.insert("Description", CustomerDataSystem::s_remoteProcedureIntrospection.value(tmpRemoteProcedureName));
        remoteProcedureInfo.insert(tmpRemoteProcedureName, tmpRemoteProcedureInfo);

        rpcData = new VeinComponent::RemoteProcedureData();
        rpcData->setEntityId(CustomerDataSystem::s_entityId);
        rpcData->setCommand(VeinComponent::RemoteProcedureData::Command::RPCMD_REGISTER);
        rpcData->setProcedureName(tmpRemoteProcedureName);
        rpcData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
        rpcData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);

        emit sigSendEvent(new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, rpcData));
    }

    introspectionRootObject.insert(QLatin1String("ComponentInfo"), componentInfo);
    introspectionRootObject.insert(QLatin1String("ProcedureInfo"), remoteProcedureInfo);
    introspectionDoc.setObject(introspectionRootObject);

    initData = new VeinComponent::ComponentData();
    initData->setEntityId(CustomerDataSystem::s_entityId);
    initData->setCommand(VeinComponent::ComponentData::Command::CCMD_ADD);
    initData->setComponentName(CustomerDataSystem::s_introspectionComponentName);
    initData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
    initData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
    initData->setNewValue(introspectionDoc.toJson());
    emit sigSendEvent(new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, initData));
    initData = nullptr;
}

void CustomerDataSystem::unloadFile(){
    VeinComponent::ComponentData *initData=nullptr;
    const QList<QString> tmpComponentList = CustomerDataSystem::s_componentIntrospection.keys();
    for(const QString &tmpComponentName : qAsConst(tmpComponentList)) {
        if(tmpComponentName != CustomerDataSystem::s_entityNameComponentName) {
            initData = new VeinComponent::ComponentData();
            initData->setEntityId(CustomerDataSystem::s_entityId);
            initData->setCommand(VeinComponent::ComponentData::Command::CCMD_SET);
            initData->setComponentName(tmpComponentName);
            initData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
            initData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
            initData->setNewValue(QString(""));
            emit sigSendEvent(new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, initData));
        }
    }
}

void CustomerDataSystem::writeCustomerdata()
{
    Q_ASSERT(QString(MODMAN_CUSTOMERDATA_PATH).isEmpty() == false);
    QSaveFile customerDataFile(QString("%1%2").arg(MODMAN_CUSTOMERDATA_PATH).arg(m_currentCustomerFileName));
    customerDataFile.open(QIODevice::WriteOnly);
    customerDataFile.write(m_currentCustomerDocument.toJson(QJsonDocument::Indented));
    if(customerDataFile.commit() == false) {
        qCritical() << "Error writing customerdata json file:" << m_currentCustomerFileName;
    }
}

void CustomerDataSystem::updateDataFile(QString t_componentName, QString t_newValue)
{
    if(m_currentCustomerFileName.isEmpty() == false) {
        QJsonObject tmpObject = m_currentCustomerDocument.object();
        const QStringList documentKeyList = tmpObject.keys();
        if(documentKeyList.contains(t_componentName)) {
            tmpObject.insert(t_componentName, t_newValue);
            m_currentCustomerDocument.setObject(tmpObject);
            m_dataWriteDelay.start(); //delay the write so other changes to the same document in short succession only write the file once
        }
        else {
            qWarning() << "Unknown data entry to add in customerdata json file:" << t_componentName;
        }
    }
}

bool CustomerDataSystem::parseCustomerDataFile(const QString &t_fileName)
{
    Q_ASSERT(QString(MODMAN_CUSTOMERDATA_PATH).isEmpty() == false);
    bool retVal = false;
    QFile customerDataFile(QString("%1%2").arg(MODMAN_CUSTOMERDATA_PATH).arg(t_fileName));
    QJsonParseError parseError;
    if(customerDataFile.exists() && customerDataFile.open(QFile::ReadOnly)) {
        m_currentCustomerFileName = t_fileName;
        m_currentCustomerDocument = QJsonDocument::fromJson(customerDataFile.readAll(), &parseError);
        if(m_currentCustomerDocument.isObject()) {
            const QJsonObject tmpObject = m_currentCustomerDocument.object();
            QStringList documentKeyList(tmpObject.keys());
            QSet<QString> entries(documentKeyList.begin(), documentKeyList.end());
            QStringList componentNameList = s_componentIntrospection.keys();
            QSet<QString> componentNames(componentNameList.begin(), componentNameList.end());
            //remove hostile entries that have no use in the file
            componentNames.remove(CustomerDataSystem::s_entityNameComponentName);
            componentNames.remove(CustomerDataSystem::s_fileSelectedComponentName);
            QSet<QString> unknownData = entries; //copy from componentNames

            unknownData.subtract(componentNames); //the files could be hand edited, unknownData will hold all unknown values
            if(unknownData.isEmpty() == false) {
                qWarning() << "Unknown data in customerdata json file:" << t_fileName << "keys:" << unknownData;
            }

            //we do not want the superfluous data
            entries.subtract(unknownData);

            if(componentNames.subtract(entries).isEmpty() == false) { //check if all component names are present
                qWarning() << "Missing values in customer data file:" << t_fileName << "values:" << componentNames;
            }

            retVal = (tmpObject.isEmpty() == false);
            for(QString componentName : qAsConst(entries)) { //best effort try to set all components
                VeinComponent::ComponentData *customerDataFieldData = new VeinComponent::ComponentData();
                customerDataFieldData->setEntityId(CustomerDataSystem::s_entityId);
                customerDataFieldData->setCommand(VeinComponent::ComponentData::Command::CCMD_SET);
                customerDataFieldData->setComponentName(componentName);
                customerDataFieldData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
                customerDataFieldData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
                customerDataFieldData->setNewValue(tmpObject.value(componentName).toString());
                emit sigSendEvent(new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, customerDataFieldData));
            }
        }
        else {
            QString errorMessage = QString("Invalid JSON data in customer data file: %1 error: %2").arg(t_fileName).arg(parseError.errorString());
            qWarning() << errorMessage;
            emit sigCustomerDataError(errorMessage);
        }
    }
    else {
        QStringList componentNameList(s_componentIntrospection.keys());
        QSet<QString> componentNames(componentNameList.begin(), componentNameList.end());
        componentNames.remove(CustomerDataSystem::s_entityNameComponentName);
        componentNames.remove(CustomerDataSystem::s_fileSelectedComponentName);
        for(QString compName : qAsConst(componentNames)) { //unset all components
            VeinComponent::ComponentData *customerDataClearData = new VeinComponent::ComponentData();
            customerDataClearData->setEntityId(CustomerDataSystem::s_entityId);
            customerDataClearData->setCommand(VeinComponent::ComponentData::Command::CCMD_SET);
            customerDataClearData->setComponentName(compName);
            customerDataClearData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
            customerDataClearData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
            customerDataClearData->setNewValue(QString());
            emit sigSendEvent(new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, customerDataClearData));
        }
    }
    return retVal;
}

void CustomerDataSystem::rpcFinished(QUuid t_callId, const QString &t_procedureName, const QVariantMap &t_data)
{
    Q_ASSERT(m_pendingRpcHash.contains(t_callId));
    VeinComponent::RemoteProcedureData *resultData = new VeinComponent::RemoteProcedureData();
    resultData->setEntityId(s_entityId);
    resultData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
    resultData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
    resultData->setCommand(VeinComponent::RemoteProcedureData::Command::RPCMD_RESULT);
    resultData->setProcedureName(t_procedureName);
    resultData->setInvokationData(t_data);

    VeinEvent::CommandEvent *rpcResultEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, resultData);
    rpcResultEvent->setPeerId(m_pendingRpcHash.value(t_callId));
    emit sigSendEvent(rpcResultEvent);
    m_pendingRpcHash.remove(t_callId);
}

void CustomerDataSystem::rpcProgress(QUuid t_callId, const QString &t_procedureName, const QVariantMap &t_data)
{
    Q_ASSERT(m_pendingRpcHash.contains(t_callId));
    VeinComponent::RemoteProcedureData *progressData = new VeinComponent::RemoteProcedureData();
    progressData->setEntityId(s_entityId);
    progressData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
    progressData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
    progressData->setCommand(VeinComponent::RemoteProcedureData::Command::RPCMD_PROGRESS);
    progressData->setProcedureName(t_procedureName);
    progressData->setInvokationData(t_data);

    VeinEvent::CommandEvent *rpcResultEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, progressData);
    rpcResultEvent->setPeerId(m_pendingRpcHash.value(t_callId));
    emit sigSendEvent(rpcResultEvent);
}

void CustomerDataSystem::customerDataAdd(const QUuid &t_callId, const QVariantMap &t_parameters)
{
    QSet<QString> requiredParamKeys = {"fileName"};
    const QVariantMap parameters = t_parameters.value(VeinComponent::RemoteProcedureData::s_parameterString).toMap();
    QVariantMap retVal = t_parameters;//copy parameters and other data, the client could attach tracking
    QStringList parameterNameList(parameters.keys());
    requiredParamKeys.subtract(QSet<QString>(parameterNameList.begin(), parameterNameList.end()));

    if(requiredParamKeys.isEmpty()) {
        const QString fileName = parameters.value("fileName").toString();
        bool parameterError = false;
        //sanitize filename
        const QStringList invalidParameters = {"\"", "|", "`", "$", "!", "/", "\\", "<", ">", ":", "?", "../", ".."};
        for(QString tmpInvalid : qAsConst(invalidParameters)) {
            if(fileName.contains(tmpInvalid)) {
                parameterError = true;
                break;
            }
        }

        if(fileName.isEmpty() || parameterError == true) {
            retVal.insert(VeinComponent::RemoteProcedureData::s_resultCodeString, RPCResultCodes::CDS_EINVAL);
            retVal.insert(VeinComponent::RemoteProcedureData::s_errorMessageString, QString("Invalid data for parameter: [fileName]"));
        }
        else {
            QFile newCustomerDataFile(QString("%1%2").arg(MODMAN_CUSTOMERDATA_PATH).arg(fileName.toLower()));
            if(newCustomerDataFile.exists() == false) {
                QFileInfo fileInfo(newCustomerDataFile);
                QFileInfo dirInfo(fileInfo.dir().absolutePath());
                Q_ASSERT(dirInfo.isWritable()); //the path should be writable if no file exists
                if(newCustomerDataFile.open(QIODevice::WriteOnly)) {
                    QJsonDocument dataDocument;
                    QJsonObject rootObject;
                    QList<QString> entries = CustomerDataSystem::s_componentIntrospection.keys();
                    //remove entries that have no use in the file
                    entries.removeAll(CustomerDataSystem::s_entityNameComponentName);
                    entries.removeAll(CustomerDataSystem::s_fileSelectedComponentName);
                    for(const QString &entryName : qAsConst(entries)) {
                        rootObject.insert(entryName, QString());
                    }
                    dataDocument.setObject(rootObject);
                    newCustomerDataFile.write(dataDocument.toJson(QJsonDocument::Indented));
                    newCustomerDataFile.close();
                    retVal.insert(VeinComponent::RemoteProcedureData::s_resultCodeString, RPCResultCodes::CDS_SUCCESS);
                }
                else {
                    retVal.insert(VeinComponent::RemoteProcedureData::s_resultCodeString, newCustomerDataFile.error());//maps to values >= ResultCodes::CDS_QFILEDEVICE_FILEERROR_BEGIN
                    retVal.insert(VeinComponent::RemoteProcedureData::s_errorMessageString, QString("Could not write customer data file: %1 error: %2").arg(newCustomerDataFile.fileName()).arg(newCustomerDataFile.errorString()));
                }
            }
            else {
                retVal.insert(VeinComponent::RemoteProcedureData::s_resultCodeString, RPCResultCodes::CDS_EEXIST);
                retVal.insert(VeinComponent::RemoteProcedureData::s_errorMessageString, QString("Customer data file already exists: %1").arg(newCustomerDataFile.fileName()));
            }
        }
    }
    else {
        retVal.insert(VeinComponent::RemoteProcedureData::s_resultCodeString, RPCResultCodes::CDS_EINVAL);
        retVal.insert(VeinComponent::RemoteProcedureData::s_errorMessageString, QString("Missing required parameters: [%1]").arg(requiredParamKeys.values().join(',')));
    }

    rpcFinished(t_callId, s_customerDataAddProcedureName, retVal);
}

void CustomerDataSystem::customerDataRemove(const QUuid &t_callId, const QVariantMap &t_parameters)
{
    QSet<QString> requiredParamKeys = {"fileName"};
    const QVariantMap parameters = t_parameters.value(VeinComponent::RemoteProcedureData::s_parameterString).toMap();
    QVariantMap retVal = parameters;//copy parameters and other data (e.g. client request tracking)
    QStringList parameterNameList(parameters.keys());
    requiredParamKeys.subtract(QSet<QString>(parameterNameList.begin(), parameterNameList.end()));

    if(requiredParamKeys.isEmpty()) {
        const QString fileName = parameters.value("fileName").toString();
        bool parameterError = false;
        //sanitize filename
        const QStringList invalidParameters = {"\"", "|", "`", "$", "!", "/", "\\", "<", ">", ":", "?", "../", ".."};
        for(QString tmpInvalid : qAsConst(invalidParameters)) {
            if(fileName.contains(tmpInvalid)) {
                parameterError = true;
                break;
            }
        }

        if(fileName.isEmpty() || parameterError == true) {
            retVal.insert(VeinComponent::RemoteProcedureData::s_resultCodeString, RPCResultCodes::CDS_EINVAL);
            retVal.insert(VeinComponent::RemoteProcedureData::s_errorMessageString, QString("Invalid data for parameter: [fileName]"));
        }
        else {
            QFile toDelete(QString("%1%2").arg(MODMAN_CUSTOMERDATA_PATH).arg(fileName));
            if(fileName.isEmpty() == false && toDelete.exists()) {
                if(toDelete.remove() == true) {
                    retVal.insert(VeinComponent::RemoteProcedureData::s_resultCodeString, RPCResultCodes::CDS_SUCCESS);
                    if(fileName == m_currentCustomerFileName) { // was it FileSelected?
                        m_currentCustomerFileName.clear();
                        unloadFile();
                    }
                }
                else {
                    retVal.insert(VeinComponent::RemoteProcedureData::s_resultCodeString, toDelete.error());//maps to values >= ResultCodes::CDS_QFILEDEVICE_FILEERROR_BEGIN
                    retVal.insert(VeinComponent::RemoteProcedureData::s_errorMessageString, QString("Could not delete customer data file: %1 error: %2").arg(toDelete.fileName()).arg(toDelete.errorString()));
                }
            }
            else {
                retVal.insert(VeinComponent::RemoteProcedureData::s_resultCodeString, RPCResultCodes::CDS_ENOENT);
                retVal.insert(VeinComponent::RemoteProcedureData::s_errorMessageString, QString("Customer data file does not exist: %1").arg(toDelete.fileName()));
            }
        }
    }
    else {
        retVal.insert(VeinComponent::RemoteProcedureData::s_resultCodeString, RPCResultCodes::CDS_EINVAL);
        retVal.insert(VeinComponent::RemoteProcedureData::s_errorMessageString, QString("Missing required parameters: [%1]").arg(requiredParamKeys.values().join(',')));
    }

    rpcFinished(t_callId, s_customerDataRemoveProcedureName, retVal);
}

void CustomerDataSystem::customerDataSearch(const QUuid &t_callId, const QVariantMap &t_parameters)
{
    QSet<QString> requiredParamKeys = {"searchMap"};
    const QVariantMap parameters = t_parameters.value(VeinComponent::RemoteProcedureData::s_parameterString).toMap();
    QStringList parameterNameList(parameters.keys());
    requiredParamKeys.subtract(QSet<QString>(parameterNameList.begin(), parameterNameList.end()));

    if(requiredParamKeys.isEmpty()) {
        QVariantMap searchParamMap = parameters.value("searchMap").toMap();
        if(searchParamMap.isEmpty() == false) {
            QFutureWatcher<QString> *tmpWatcher = new QFutureWatcher<QString>();
            m_pendingSearchResultWatchers.insert(t_callId, tmpWatcher);
            connect(tmpWatcher, &QFutureWatcher<QString>::resultReadyAt, [this, t_callId, t_parameters, tmpWatcher](int t_resultPos){
                QVariantMap tmpData = t_parameters;//writable copy
                tmpData.insert(s_customerDataSearchResultText, tmpWatcher->future().resultAt(t_resultPos));
                rpcProgress(t_callId, s_customerDataSearchProcedureName, tmpData);
            });
            connect(tmpWatcher, &QFutureWatcher<QString>::finished, [this, t_callId, t_parameters, tmpWatcher](){
                m_pendingSearchResultWatchers.remove(t_callId);
                QVariantMap retVal = t_parameters; //writable copy
                retVal.insert(VeinComponent::RemoteProcedureData::s_resultCodeString, RPCResultCodes::CDS_SUCCESS);
                rpcFinished(t_callId, s_customerDataSearchProcedureName, retVal);
                tmpWatcher->deleteLater();
            });
            ///@todo make cancel accessible via rpc
            connect(tmpWatcher, &QFutureWatcher<QString>::canceled, [this, t_callId, t_parameters, tmpWatcher](){
                m_pendingSearchResultWatchers.remove(t_callId);
                QVariantMap retVal = t_parameters; //writable copy
                retVal.insert(VeinComponent::RemoteProcedureData::s_resultCodeString, RPCResultCodes::CDS_CANCELED);
                rpcFinished(t_callId, s_customerDataSearchProcedureName, retVal);
                tmpWatcher->deleteLater();
            });
            const QStringList entryList = QDir(MODMAN_CUSTOMERDATA_PATH, QStringLiteral("*.json")).entryList();
            QFuture<QString> searchResultStream = QtConcurrent::filtered(entryList, [searchParamMap](const QString &t_fileName){
                bool retVal = true;
                QFile jsonFile(QString("%1%2").arg(MODMAN_CUSTOMERDATA_PATH).arg(t_fileName));
                jsonFile.open(QFile::ReadOnly);
                QJsonParseError parseError;
                QJsonDocument currentDocument = QJsonDocument::fromJson(jsonFile.readAll(), &parseError);
                if(parseError.error == QJsonParseError::NoError && currentDocument.isObject()) {
                    const QJsonObject jsonObj = currentDocument.object();
                    const QStringList searchKeys =  searchParamMap.keys();
                    for(const QString &currentKey : qAsConst(searchKeys)) {
                        Q_ASSERT(jsonObj.contains(currentKey));
                        QRegExp searchRegexp(searchParamMap.value(currentKey).toString(), Qt::CaseInsensitive);
                        if(jsonObj.value(currentKey).toString().contains(searchRegexp)) {
                            retVal = true;
                        }
                        else { //currently only logical AND matching is supported
                            retVal = false;
                            break;
                        }
                    }
                }
                else {
                    retVal = false;
                }
                return retVal;
            });
            tmpWatcher->setFuture(searchResultStream);
        }
    }
    else
    {
        QVariantMap retVal = t_parameters;//writable copy
        retVal.insert(VeinComponent::RemoteProcedureData::s_resultCodeString, RPCResultCodes::CDS_EINVAL);
        retVal.insert(VeinComponent::RemoteProcedureData::s_errorMessageString, QString("Missing required parameters: [%1]").arg(requiredParamKeys.values().join(',')));
        rpcFinished(t_callId, s_customerDataSearchProcedureName, retVal);
    }
}

//constexpr definition, see: https://stackoverflow.com/questions/8016780/undefined-reference-to-static-constexpr-char
constexpr QLatin1String CustomerDataSystem::s_entityName;
constexpr QLatin1String CustomerDataSystem::s_entityNameComponentName;
constexpr QLatin1String CustomerDataSystem::s_entityNameComponentDescription;
constexpr QLatin1String CustomerDataSystem::s_introspectionComponentName;
constexpr QLatin1String CustomerDataSystem::s_introspectionComponentDescription;
constexpr QLatin1String CustomerDataSystem::s_fileSelectedComponentName;
constexpr QLatin1String CustomerDataSystem::s_fileSelectedComponentDescription;
//rpc
constexpr QLatin1String CustomerDataSystem::s_customerDataAddProcedureName;
constexpr QLatin1String CustomerDataSystem::s_customerDataAddProcedureDescription;
constexpr QLatin1String CustomerDataSystem::s_customerDataRemoveProcedureName;
constexpr QLatin1String CustomerDataSystem::s_customerDataRemoveProcedureDescription;
constexpr QLatin1String CustomerDataSystem::s_customerDataSearchProcedureName;
constexpr QLatin1String CustomerDataSystem::s_customerDataSearchProcedureDescription;
constexpr QLatin1String CustomerDataSystem::s_customerDataSearchResultText;
constexpr QLatin1String CustomerDataSystem::s_cusomerDataRpcProgress;
//base
constexpr QLatin1String CustomerDataSystem::s_baseIdentifierComponentName;
constexpr QLatin1String CustomerDataSystem::s_baseIdentifierComponentDescription;
constexpr QLatin1String CustomerDataSystem::s_baseCommentComponentName;
constexpr QLatin1String CustomerDataSystem::s_baseCommentComponentDescription;
//customer
constexpr QLatin1String CustomerDataSystem::s_customerFirstNameComponentName;
constexpr QLatin1String CustomerDataSystem::s_customerFirstNameComponentDescription;
constexpr QLatin1String CustomerDataSystem::s_customerLastNameComponentName;
constexpr QLatin1String CustomerDataSystem::s_customerLastNameComponentDescription;
constexpr QLatin1String CustomerDataSystem::s_customerStreetComponentName;
constexpr QLatin1String CustomerDataSystem::s_customerStreetComponentDescription;
constexpr QLatin1String CustomerDataSystem::s_customerPostalCodeComponentName;
constexpr QLatin1String CustomerDataSystem::s_customerPostalCodeComponentDescription;
constexpr QLatin1String CustomerDataSystem::s_customerCityComponentName;
constexpr QLatin1String CustomerDataSystem::s_customerCityComponentDescription;
constexpr QLatin1String CustomerDataSystem::s_customerCountryComponentName;
constexpr QLatin1String CustomerDataSystem::s_customerCountryComponentDescription;
constexpr QLatin1String CustomerDataSystem::s_customerNumberComponentName;
constexpr QLatin1String CustomerDataSystem::s_customerNumberComponentDescription;
constexpr QLatin1String CustomerDataSystem::s_customerCommentComponentName;
constexpr QLatin1String CustomerDataSystem::s_customerCommentComponentDescription;
//location
constexpr QLatin1String CustomerDataSystem::s_locationFirstNameComponentName;
constexpr QLatin1String CustomerDataSystem::s_locationFirstNameComponentDescription;
constexpr QLatin1String CustomerDataSystem::s_locationLastNameComponentName;
constexpr QLatin1String CustomerDataSystem::s_locationLastNameComponentDescription;
constexpr QLatin1String CustomerDataSystem::s_locationStreetComponentName;
constexpr QLatin1String CustomerDataSystem::s_locationStreetComponentDescription;
constexpr QLatin1String CustomerDataSystem::s_locationPostalCodeComponentName;
constexpr QLatin1String CustomerDataSystem::s_locationPostalCodeComponentDescription;
constexpr QLatin1String CustomerDataSystem::s_locationCityComponentName;
constexpr QLatin1String CustomerDataSystem::s_locationCityComponentDescription;
constexpr QLatin1String CustomerDataSystem::s_locationCountryComponentName;
constexpr QLatin1String CustomerDataSystem::s_locationCountryComponentDescription;
constexpr QLatin1String CustomerDataSystem::s_locationNumberComponentName;
constexpr QLatin1String CustomerDataSystem::s_locationNumberComponentDescription;
constexpr QLatin1String CustomerDataSystem::s_locationCommentComponentName;
constexpr QLatin1String CustomerDataSystem::s_locationCommentComponentDescription;
//powerGrid
constexpr QLatin1String CustomerDataSystem::s_powerGridOperatorComponentName;
constexpr QLatin1String CustomerDataSystem::s_powerGridOperatorComponentDescription;
constexpr QLatin1String CustomerDataSystem::s_powerGridSupplierComponentName;
constexpr QLatin1String CustomerDataSystem::s_powerGridSupplierComponentDescription;
constexpr QLatin1String CustomerDataSystem::s_powerGridCommentComponentName;
constexpr QLatin1String CustomerDataSystem::s_powerGridCommentComponentDescription;
//meter
constexpr QLatin1String CustomerDataSystem::s_meterManufacturerComponentName;
constexpr QLatin1String CustomerDataSystem::s_meterManufacturerComponentDescription;
constexpr QLatin1String CustomerDataSystem::s_meterFactoryNumberComponentName;
constexpr QLatin1String CustomerDataSystem::s_meterFactoryNumberComponentDescription;
constexpr QLatin1String CustomerDataSystem::s_meterOwnerComponentName;
constexpr QLatin1String CustomerDataSystem::s_meterOwnerComponentDescription;
constexpr QLatin1String CustomerDataSystem::s_meterCommentComponentName;
constexpr QLatin1String CustomerDataSystem::s_meterCommentComponentDescription;
