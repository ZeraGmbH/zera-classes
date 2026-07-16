#include "scpiclient.h"
#include "scpimodule.h"
#include "scpimoduleconfigdata.h"
#include "statusbitdescriptor.h"
#include "scpiserver.h"
#include "ieee488-2-definitions.h"
#include "scpimodelmeasure.h"
#include <zscpi_response_definitions.h>
#include <vs_abstracteventsystem.h>
#include <QJsonObject>
#include <QJsonDocument>

namespace SCPIMODULE
{
cSCPIClient::cSCPIClient(cSCPIModule* module) :
    m_pModule(module),
    m_pSCPIInterface(module->getScpiInterface()),
    m_ConfigData(*module->getConfigData()),
    m_clientId(QUuid::createUuid()) // we need an unique id in case we want to send deferred error notifications events
{
    // we instantiate 3 scpi status systems per client
    cSCPIStatus* scpiQuestStatus = new cSCPIStatus(STBques);
    m_SCPIStatusList.append(scpiQuestStatus);
    cSCPIStatus* scpiOperStatus = new cSCPIStatus(STBoper);
    m_SCPIStatusList.append(scpiOperStatus);
    cSCPIStatus* scpiOperMeasStatus = new cSCPIStatus(OperationMeasureSummary);
    m_SCPIStatusList.append(scpiOperMeasStatus);

    m_pIEEE4882 = new cIEEE4882(this, m_ConfigData.m_sDeviceName, module->getStorageDb());

    // we connect the cascaded scpi operation status systems
    connect(scpiOperMeasStatus, &cSCPIStatus::sigEvent, scpiOperStatus, &cSCPIStatus::SetConditionBit);
    // and we connect operationstatus and questionable status with ieee488 status byte
    connect(scpiOperStatus, &cSCPIStatus::sigEvent, m_pIEEE4882, &cIEEE4882::setStatusByte);
    connect(scpiQuestStatus, &cSCPIStatus::sigEvent, m_pIEEE4882, &cIEEE4882::setStatusByte);

    // and we need this connections for setting status conditions as result of common commands
    connect(m_pIEEE4882, &cIEEE4882::setQuestionableCondition, scpiQuestStatus, &cSCPIStatus::setCondition);
    connect(m_pIEEE4882, &cIEEE4882::setOperationCondition, scpiOperStatus, &cSCPIStatus::setCondition);
    connect(m_pIEEE4882, &cIEEE4882::setOperationMeasureCondition, scpiOperMeasStatus, &cSCPIStatus::setCondition);

    // and we must connect event error signals of scpi status systems to common status
    connect(scpiQuestStatus, &cSCPIStatus::sigEventErrorCmdFinish, m_pIEEE4882, &cIEEE4882::addEventErrorWithResponse);
    connect(scpiOperStatus, &cSCPIStatus::sigEventErrorCmdFinish, m_pIEEE4882, &cIEEE4882::addEventErrorWithResponse);
    connect(scpiOperMeasStatus, &cSCPIStatus::sigEventErrorCmdFinish, m_pIEEE4882, &cIEEE4882::addEventErrorWithResponse);

    const VeinScpiModuleInterfaceParser::ScpiEntityHash entitiesWithScpi = m_pModule->getScpiModuleInterfaceParser().getEntitiesWithScpi();
    setSignalConnections(scpiQuestStatus, m_ConfigData.m_QuestionableStatDescriptorList, entitiesWithScpi);
    setSignalConnections(scpiOperStatus, m_ConfigData.m_OperationStatDescriptorList, entitiesWithScpi);
    setSignalConnections(scpiOperMeasStatus, m_ConfigData.m_OperationMeasureStatDescriptorList, entitiesWithScpi);

    generateSCPIMeasureSystem();
}

cSCPIClient::~cSCPIClient()
{
    for (int i = 0; i < m_connectDelegateList.count(); i++) {
        cSignalConnectionDelegate* sCD = m_connectDelegateList.at(i);
        m_pModule->removeSignalConnectionDelegates(sCD);
        delete sCD;
    }
    for (int i = 0; i < m_SCPIStatusList.count(); i++)
        delete m_SCPIStatusList.at(i);
    delete m_pIEEE4882;

    QList<VeinComponentScpiMeasureSequence*> keylist;
    keylist = m_SCPIMeasureTranslationHash.keys();
    for (int i = 0; i < keylist.count(); i++)
        delete m_SCPIMeasureTranslationHash[keylist.at(i)];
}

cSCPIInterface *cSCPIClient::getScpiInterface()
{
    return m_pSCPIInterface;
}

cSCPIStatus *cSCPIClient::getSCPIStatus(SCPIStatusDefinitions::ScpiStatusSystems statusSystemIdx)
{
    return m_SCPIStatusList.at(statusSystemIdx);
}

bool cSCPIClient::isOperationComplete()
{
    return m_veinParamRpcTransactionHash.isEmpty();
}

cIEEE4882 *cSCPIClient::getIEEE4882()
{
    return m_pIEEE4882;
}

void cSCPIClient::addVeinParamRpcTransactionInfo(const QString &veinComponentOrRpcName, const SCPIVeinTransactionInfoPtr &info)
{
    m_veinParamRpcTransactionHash.insert(veinComponentOrRpcName, info);
}

void cSCPIClient::removeVeinParamRpcTransactionInfo(const QString &veinComponentOrRpcName)
{
    m_veinParamRpcTransactionHash.remove(veinComponentOrRpcName);
    execCmd();
}

int cSCPIClient::execPendingCmds()
{
    int cmdsStarted = 0;
    while (cmdAvail()) {
        // if we have complete commands
        takeCmd(); // we fetch 1 of them
        execCmd(); // and execute it
        cmdsStarted++;
    }
    return cmdsStarted;
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
    if(m_activeCmd.isEmpty()) {
        if (m_sInputFifo.contains('\n'))
            m_endChar = '\n';
        else
            m_endChar = '\r';
        int index = m_sInputFifo.indexOf(m_endChar);
        m_activeCmd = m_sInputFifo.left(index);
        m_activeCmd.remove('\n'); // we don't know which was the first
        m_activeCmd.remove('\r');
        m_activeCmd.remove('\t');
        m_sInputFifo.remove(0, index+1);
        if (m_sInputFifo.length() > 0) {
            QChar firstChar = m_sInputFifo.at(0); // maybe there is still 1 end char
            if ((firstChar == '\n') or (firstChar == '\r'))
                m_sInputFifo.remove(0,1);
        }
    }
}

void cSCPIClient::execCmd()
{
    while (true) {
        QStringList cmdList = m_activeCmd.split('|');
        QString cmd = cmdList.at(0);
        m_activeCmd.remove(0, cmd.length());
        if (m_activeCmd.length() > 0) {
            if (m_activeCmd.at(0) == '|')
               m_activeCmd.remove(0,1);
        }
        // we will leave if there is no command anymore
        if (cmd.length() == 0)
            break;

        // This message is checked as is in autobuilder-dut-testsuite!
        qInfo("Executing SCPI command : %s", qPrintable(cmd));

        ScpiTransactionId scpiTransactionId = m_responseSorter.createTransaction(cmd);
        if (!m_pSCPIInterface->executeCmd(this, cmd, scpiTransactionId))
            m_pIEEE4882->addEventErrorWithResponse(CommandError, scpiTransactionId);
        // we leave here if there is any parameter settings pending
        if (m_veinParamRpcTransactionHash.count() > 0)
            break;
    }
}

QString cSCPIClient::makeBareScpiInPrintable(const QString &input)
{
    QString ret = input;
    ret.replace("\n", "\\n");
    ret.replace("\r", "\\r");
    ret.replace("\t", "\\t");
    return ret;
}

QUuid cSCPIClient::getClientId()
{
    return m_clientId;
}

void cSCPIClient::handleCmdFinishStatusOnly(quint8 stat, const ScpiTransactionId &scpiTransactionId)
{
    switch (stat)
    {
    case ZSCPI::ack:
        handleCmdFinish(NullableString(), scpiTransactionId);
        break;
    case ZSCPI::nak: // e.g SCPI command improper terminated / RPC too many params
        m_pIEEE4882->addEventErrorWithResponse(CommandError, scpiTransactionId);
        break;
    case ZSCPI::errval: // standard vein component error
        m_pIEEE4882->addEventErrorWithResponse(NumericDataError, scpiTransactionId);
        break;
    case ZSCPI::erraut: // unused?
        m_pIEEE4882->addEventErrorWithResponse(CommandProtected, scpiTransactionId);
        break;
    case ZSCPI::errexec: // e.g. RPC
        m_pIEEE4882->addEventErrorWithResponse(ExecutionError, scpiTransactionId);
        break;
    default: // unused?
        m_pIEEE4882->addEventErrorWithResponse(CommandError, scpiTransactionId);
        break;
    }
}

void cSCPIClient::setSignalConnections(cSCPIStatus* scpiStatus,
                                       const QList<cStatusBitDescriptor> &statusBitDescriptorList,
                                       const VeinScpiModuleInterfaceParser::ScpiEntityHash &entitiesWithScpi)
{
    if (entitiesWithScpi.isEmpty())
        return;

    int descriptorCount = statusBitDescriptorList.count();
    if (descriptorCount <= 0)
        return;

    const VeinStorage::AbstractDatabase* storageDb = m_pModule->getStorageDb();
    for (int descriptorIdx = 0; descriptorIdx < descriptorCount; descriptorIdx++) {
        cStatusBitDescriptor statusBitDescriptor = statusBitDescriptorList.at(descriptorIdx);
        const QString &moduleName = statusBitDescriptor.m_sSCPIModuleName;
        if (entitiesWithScpi.contains(moduleName)) {
            const int entityId = entitiesWithScpi[moduleName];
            if (storageDb->hasStoredValue(entityId, statusBitDescriptor.m_sComponentName)) {
                // if we found the searched component, we generate a signal connection delegate
                // we need an eventsystem to look for notifications with these components
                // that lets the signal connection delegate  do his job
                cSignalConnectionDelegate* delegate = new cSignalConnectionDelegate(scpiStatus,
                                                                                    statusBitDescriptor.m_nBitNr,
                                                                                    entityId, statusBitDescriptor.m_sComponentName);
                // as we only want a single eventsystem the module itself holds the list of delegates
                m_pModule->insertSignalConnectionDelegate(delegate);
                m_connectDelegateList.append(delegate); // our own list so we can clean up if client dies
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
    QHash<QString, ScpiDelegateMeasurePtr> *pSCPIMeasDelegateHash = m_pModule->getScpiModelMeasurement()->getSCPIMeasDelegateHash();
    QList<QString> keylist = pSCPIMeasDelegateHash->keys();
    for (int i = 0; i < keylist.count(); i++) {
        ScpiDelegateMeasurePtr measDelegate = (*pSCPIMeasDelegateHash)[keylist.at(i)];
        m_SCPIMeasureDelegateHash[measDelegate.get()] = std::make_shared<ScpiClientMeasureExecutor>(*measDelegate, m_SCPIMeasureTranslationHash);
    }
}

}
