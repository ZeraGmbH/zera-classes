#include "scpimodule.h"
#include "ieee488-2.h"
#include "scpiclient.h"
#include "scpiinterface.h"
#include "scpistatus.h"
#include "scpimoduleconfigdata.h"
#include "signalconnectiondelegate.h"
#include "statusbitdescriptor.h"
#include "scpiserver.h"
#include "moduleinterface.h"
#include "scpimeasuredelegate.h"
#include "scpimeasure.h"
#include <scpi.h>
#include <zscpi_response_definitions.h>
#include <ve_storagesystem.h>
#include <QJsonDocument>

namespace SCPIMODULE
{
cSCPIClient::cSCPIClient(cSCPIModule* module, cSCPIModuleConfigData &configdata, cSCPIInterface* iface) :
    m_pSCPIInterface(iface),
    m_pModule(module),
    m_ConfigData(configdata)
{
    mClientId = QUuid::createUuid(); // we need an unique id in case we want to send deferred error notifications events

    m_bAuthorisation = false;
    m_sInputFifo = "";

    // we instantiate 3 scpi status systems per client
    cSCPIStatus* scpiQuestStatus;
    cSCPIStatus* scpiOperStatus;
    cSCPIStatus* scpiOperMeasStatus;

    scpiQuestStatus = new cSCPIStatus(STBques);
    m_SCPIStatusList.append(scpiQuestStatus);
    scpiOperStatus = new cSCPIStatus(STBoper);
    m_SCPIStatusList.append(scpiOperStatus);
    scpiOperMeasStatus = new cSCPIStatus(OperationMeasureSummary);
    m_SCPIStatusList.append(scpiOperMeasStatus);

    m_pIEEE4882 = new cIEEE4882(this, m_ConfigData.m_sDeviceName, 50);

    // we connect the cascaded scpi operation status systems
    connect(scpiOperMeasStatus, &cSCPIStatus::event, scpiOperStatus, &cSCPIStatus::SetConditionBit);
    // and we connect operationstatus and questionable status with ieee488 status byte
    connect(scpiOperStatus, &cSCPIStatus::event, m_pIEEE4882, &cIEEE4882::setStatusByte);
    connect(scpiQuestStatus, &cSCPIStatus::event, m_pIEEE4882, &cIEEE4882::setStatusByte);

    // and we need this connections for setting status conditions as result of common commands
    connect(m_pIEEE4882, &cIEEE4882::setQuestionableCondition, scpiQuestStatus, &cSCPIStatus::setCondition);
    connect(m_pIEEE4882, &cIEEE4882::setOperationCondition, scpiOperStatus, &cSCPIStatus::setCondition);
    connect(m_pIEEE4882, &cIEEE4882::setOperationMeasureCondition, scpiOperMeasStatus, &cSCPIStatus::setCondition);

    // and we must connect event error signals of scpi status systems to common status
    connect(scpiQuestStatus, &cSCPIStatus::eventError, m_pIEEE4882, &cIEEE4882::AddEventError);
    connect(scpiOperStatus, &cSCPIStatus::eventError, m_pIEEE4882, &cIEEE4882::AddEventError);
    connect(scpiOperMeasStatus, &cSCPIStatus::eventError, m_pIEEE4882, &cIEEE4882::AddEventError);

    setSignalConnections(scpiQuestStatus, m_ConfigData.m_QuestionableStatDescriptorList);
    setSignalConnections(scpiOperStatus, m_ConfigData.m_OperationStatDescriptorList);
    setSignalConnections(scpiOperMeasStatus, m_ConfigData.m_OperationMeasureStatDescriptorList);

    generateSCPIMeasureSystem();
}

cSCPIClient::~cSCPIClient()
{
    for (int i = 0; i < mysConnectDelegateList.count(); i++) {
        cSignalConnectionDelegate* sCD = mysConnectDelegateList.at(i);
        m_pModule->sConnectDelegateList.removeAll(sCD);
        delete sCD;
    }
    for (int i = 0; i < m_SCPIStatusList.count(); i++)
        delete m_SCPIStatusList.at(i);
    delete m_pIEEE4882;

    QList<cSCPIMeasure*> keylist;
    keylist = m_SCPIMeasureTranslationHash.keys();
    for (int i = 0; i < keylist.count(); i++)
        delete m_SCPIMeasureTranslationHash[keylist.at(i)];
    for(auto measDelegate : qAsConst(m_SCPIMeasureDelegateHash))
        delete measDelegate;
}

void cSCPIClient::setAuthorisation(bool auth)
{
    m_bAuthorisation = auth;
}

cSCPIStatus *cSCPIClient::getSCPIStatus(quint8 index)
{
    return m_SCPIStatusList.at(index);
}

quint8 cSCPIClient::operationComplete()
{
    if (m_scpiClientInfoHash.isEmpty())
        return 1;
    return 0;
}

cIEEE4882 *cSCPIClient::getIEEE4882()
{
    return m_pIEEE4882;
}

void cSCPIClient::addSCPIClientInfo(QString key, SCPIClientInfoPtr info)
{
    m_scpiClientInfoHash.insert(key, info);
}

void cSCPIClient::removeSCPIClientInfo(QString key)
{
    m_scpiClientInfoHash.remove(key);
    execCmd();
}

void cSCPIClient::testCmd()
{
    while (cmdAvail()) {
        // if we have complete commands
        takeCmd(); // we fetch 1 of them
        execCmd(); // and execute it
    }
}

bool cSCPIClient::cmdAvail()
{
    if (m_sInputFifo.contains("\n"))
        return true;
    if (m_sInputFifo.contains("\r"))
        return true;
    return false;
}

void cSCPIClient::takeCmd()
{
    if (m_sInputFifo.contains('\n'))
        endChar = '\n';
    else
        endChar = '\r';
    int index = m_sInputFifo.indexOf(endChar);
    activeCmd = m_sInputFifo.left(index);
    activeCmd.remove('\n'); // we don't know which was the first
    activeCmd.remove('\r');
    activeCmd.remove('\t');
    m_sInputFifo.remove(0, index+1);
    if (m_sInputFifo.length() > 0) {
        QChar firstChar = m_sInputFifo.at(0); // maybe there is still 1 end char
        if ((firstChar == '\n') or (firstChar == '\r'))
            m_sInputFifo.remove(0,1);
    }
}

void cSCPIClient::execCmd()
{
    while (true) {
        QStringList cmdList = activeCmd.split('|');
        QString cmd = cmdList.at(0);
        activeCmd.remove(0, cmd.length());
        if (activeCmd.length() > 0) {
            if (activeCmd.at(0) == '|')
               activeCmd.remove(0,1);
        }
        // we will leave if there is no command anymore
        if (cmd.length() == 0)
            break;
        if (!m_pSCPIInterface->executeCmd(this, cmd))
            emit m_pIEEE4882->AddEventError(CommandError);
        // we leave here if there is any parameter settings pending
        if (m_scpiClientInfoHash.count() > 0)
            break;
    }
}

QUuid cSCPIClient::getClientId()
{
    return mClientId;
}

void cSCPIClient::receiveStatus(quint8 stat)
{
    switch (stat)
    {
    case ZSCPI::ack:
        break;
    case ZSCPI::nak:
        m_pIEEE4882->AddEventError(CommandError);
        break;
    case ZSCPI::errval:
        m_pIEEE4882->AddEventError(NumericDataError);
        break;
    case ZSCPI::erraut:
        m_pIEEE4882->AddEventError(CommandProtected);
        break;
    default:
        m_pIEEE4882->AddEventError(CommandError);
        break;
    }
    emit commandAnswered(this);
}

void cSCPIClient::setSignalConnections(cSCPIStatus* scpiStatus, QList<cStatusBitDescriptor> &dList)
{
    int n;
    if ((n = dList.count()) > 0) {

        QList<int> entityIdList;
        entityIdList = m_pModule->getStorageSystem()->getEntityList();
        int entityIdCount = entityIdList.count();
        // we iterate over all statusbitdescriptors
        for (int i = 0; i < n; i++) {
            bool moduleFound = false;
            cStatusBitDescriptor des = dList.at(i); // the searched status bit descriptor
            if (entityIdCount  > 0) {
                int entityID;
                // we parse over all moduleinterface components
                for (int j = 0; j < entityIdCount; j++) {
                    entityID = entityIdList.at(j);
                    if (m_pModule->getStorageSystem()->hasStoredValue(entityID, QString("INF_ModuleInterface"))) {
                        QJsonDocument jsonDoc = QJsonDocument::fromJson(m_pModule->getStorageSystem()->getStoredValue(entityID, QString("INF_ModuleInterface")).toByteArray());
                        if ( !jsonDoc.isNull() && jsonDoc.isObject() ) {
                            QJsonObject jsonObj = jsonDoc.object();
                            jsonObj = jsonObj["SCPIInfo"].toObject();
                            QString scpiModuleName = jsonObj["Name"].toString();
                            if (scpiModuleName == des.m_sSCPIModuleName) {
                                moduleFound = true;
                                break;
                            }
                        }
                    }
                }
                if (moduleFound) {
                    if (m_pModule->getStorageSystem()->hasStoredValue(entityID, des.m_sComponentName)) {
                        // if we found the searched component, we generate a signal connection delegate
                        // we need an eventsystem to look for notifications with these components
                        // that lets the signal connection delegate  do his job

                        cSignalConnectionDelegate* sConnectDelegate;
                        sConnectDelegate = new cSignalConnectionDelegate(scpiStatus, des.m_nBitNr, entityID, des.m_sComponentName);
                        // as we only want a single eventsystem the module itself holds the list of delegates
                        m_pModule->sConnectDelegateList.append(sConnectDelegate);
                        mysConnectDelegateList.append(sConnectDelegate); // our own list so we can clean up if client dies
                    }
                }
            }
        }
    }
}

void cSCPIClient::generateSCPIMeasureSystem()
{
    // here we generate cSCPIMeasureDelegate objects including cSCPIMeasure objects for the new
    // client so that they can work independantly when querying measuring values
    // we ask the moduleinterface for the "base" cSCPIMeasureDelegate hash, that is once built up
    // on module instanciation and that are connected to the scpi interface
    QHash<QString, cSCPIMeasureDelegate *> *pSCPIMeasDelegateHash = m_pModule->getSCPIServer()->getModuleInterface()->getSCPIMeasDelegateHash();
    QList<QString> keylist = pSCPIMeasDelegateHash->keys();
    for (int i = 0; i < keylist.count(); i++) {
        cSCPIMeasureDelegate* measDelegate = (*pSCPIMeasDelegateHash)[keylist.at(i)];
        m_SCPIMeasureDelegateHash[measDelegate] = new cSCPIMeasureDelegate(*measDelegate, m_SCPIMeasureTranslationHash);
    }
}

}
