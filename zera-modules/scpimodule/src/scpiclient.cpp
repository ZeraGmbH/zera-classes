#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>

#include <scpi.h>
#include <ve_storagesystem.h>

#include "scpimodule.h"
#include "ieee488-2.h"
#include "scpiclient.h"
#include "scpiinterface.h"
#include "scpistatus.h"
#include "scpimoduleconfigdata.h"
#include "signalconnectiondelegate.h"
#include "statusbitdescriptor.h"
#include "scpieventsystem.h"


namespace SCPIMODULE
{


cSCPIClient::cSCPIClient(cSCPIModule* module, cSCPIModuleConfigData &configdata, cSCPIInterface* iface)
    :m_pModule(module), m_ConfigData(configdata), m_pSCPIInterface(iface)
{
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

    m_pIEEE4882 = new cIEEE4882(m_pModule, this, m_ConfigData.m_sDeviceIdentification, 50);

    // we connect the cascaded scpi operation status systems
    connect(scpiOperMeasStatus, SIGNAL(event(quint8, quint8)), scpiOperStatus, SLOT(SetConditionBit(quint8, quint8)));
    // and we connect operationstatus and questionable status with ieee488 status byte
    connect(scpiOperStatus, SIGNAL(event(quint8, quint8)), m_pIEEE4882, SLOT(setStatusByte(quint8, quint8)));
    connect(scpiQuestStatus, SIGNAL(event(quint8, quint8)), m_pIEEE4882, SLOT(setStatusByte(quint8, quint8)));

    // and we need this connections for setting status conditions as result of common commands
    connect(m_pIEEE4882, SIGNAL(setQuestionableCondition(quint16)), scpiQuestStatus, SLOT(setCondition(quint16)));
    connect(m_pIEEE4882, SIGNAL(setOperationCondition(quint16)), scpiOperStatus, SLOT(setCondition(quint16)));
    connect(m_pIEEE4882, SIGNAL(setOperationMeasureCondition(quint16)), scpiOperMeasStatus, SLOT(setCondition(quint16)));

    // and we must connect event error signals of scpi status systems to common status
    connect(scpiQuestStatus, SIGNAL(eventError(int)), m_pIEEE4882, SLOT(AddEventError(int)));
    connect(scpiOperStatus, SIGNAL(eventError(int)), m_pIEEE4882, SLOT(AddEventError(int)));
    connect(scpiOperMeasStatus, SIGNAL(eventError(int)), m_pIEEE4882, SLOT(AddEventError(int)));

    setSignalConnections(scpiQuestStatus, m_ConfigData.m_QuestionableStatDescriptorList);
    setSignalConnections(scpiOperStatus, m_ConfigData.m_OperationStatDescriptorList);
    setSignalConnections(scpiOperMeasStatus, m_ConfigData.m_OperationMeasureStatDescriptorList);

    connect(m_pModule->m_pSCPIEventSystem, SIGNAL(status(quint8)), this, SLOT(receiveStatus(quint8)));
//    connect(m_pModule->m_pSCPIEventSystem, SIGNAL(clientinfoSignal(QString)), this, SLOT(removeSCPIClientInfo(QString)));
}


cSCPIClient::~cSCPIClient()
{

    for (int i = 0; i < mysConnectDelegateList.count(); i++)
    {
        cSignalConnectionDelegate* sCD;
        sCD = mysConnectDelegateList.at(i);
        m_pModule->sConnectDelegateList.removeAll(sCD);
        delete sCD;
    }

    for (int i = 0; i < m_SCPIStatusList.count(); i++)
        delete m_SCPIStatusList.at(i);

    delete m_pIEEE4882;
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
    if (scpiClientInfoHash.isEmpty())
        return 1;
    else
        return 0;
}


cIEEE4882 *cSCPIClient::getIEEE4882()
{
    return m_pIEEE4882;
}


void cSCPIClient::addSCPIClientInfo(QString key, cSCPIClientInfo *info)
{
    scpiClientInfoHash.insert(key, info);
}


void cSCPIClient::removeSCPIClientInfo(QString key)
{
    scpiClientInfoHash.remove(key);
    execCmd();
}


void cSCPIClient::testCmd()
{
    while (cmdAvail())
    {
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
    QChar firstChar;
    int index;

    if (m_sInputFifo.contains('\n'))
        endChar = '\n';
    else
        endChar = '\r';

    index = m_sInputFifo.indexOf(endChar);
    activeCmd = m_sInputFifo.left(index);
    activeCmd.remove('\n'); // we don't know which was the first
    activeCmd.remove('\r');

    m_sInputFifo.remove(0, index+1);

    if (m_sInputFifo.length() > 0)
    {
        firstChar = m_sInputFifo.at(0); // maybe there is still 1 end char
        if ((firstChar == '\n') or (firstChar == '\r'))
            m_sInputFifo.remove(0,1);
    }
}


void cSCPIClient::execCmd()
{
    QStringList cmdList;
    QString cmd;

    while (true)
    {
        cmdList = activeCmd.split('|');

        cmd = cmdList.at(0);
        activeCmd.remove(0, cmd.length());
        if (activeCmd.length() > 0)
        {
            if (activeCmd.at(0) == '|')
               activeCmd.remove(0,1);
        }

        // we will leave if there is no command anymore
        if (cmd.length() == 0)
            break;

        if (!m_pSCPIInterface->executeCmd(this, cmd))
            emit m_pIEEE4882->AddEventError(CommandError);

        // we leave here if there is any parameter settings pending
        if (scpiClientInfoHash.count() > 0)
            break;
    }

}

void cSCPIClient::receiveStatus(quint8 stat)
{
    switch (stat)
    {
    case SCPI::ack:
        break;
    case SCPI::nak:
        m_pIEEE4882->AddEventError(CommandError);
        break;
    case SCPI::errval:
        m_pIEEE4882->AddEventError(NumericDataError);
        break;
    case SCPI::erraut:
        m_pIEEE4882->AddEventError(CommandProtected);
        break;
    default:
        m_pIEEE4882->AddEventError(CommandError);
        break;
    }
}


void cSCPIClient::setSignalConnections(cSCPIStatus* scpiStatus, QList<cStatusBitDescriptor> &dList)
{
    int n;

    if ((n = dList.count()) > 0)
    {

        QList<int> entityIdList;
        entityIdList = m_pModule->m_pStorageSystem->getEntityList();
        int entityIdCount = entityIdList.count();

        // we iterate over all statusbitdescriptors
        for (int i = 0; i < n; i++)
        {
            bool moduleFound;
            cStatusBitDescriptor des;

            moduleFound = false;
            des = dList.at(i); // the searched status bit descriptor

            if (entityIdCount  > 0)
            {
                int entityID;
                // we parse over all moduleinterface components
                for (int j = 0; j < entityIdCount; j++)
                {
                    entityID = entityIdList.at(j);
                    if (m_pModule->m_pStorageSystem->hasStoredValue(entityID, QString("INF_ModuleInterface")))
                    {
                        QJsonDocument jsonDoc;
                        QJsonObject jsonObj;
                        QString scpiModuleName;

                        jsonDoc = QJsonDocument::fromJson(m_pModule->m_pStorageSystem->getStoredValue(entityID, QString("INF_ModuleInterface")).toByteArray());

                        if ( !jsonDoc.isNull() && jsonDoc.isObject() )
                        {
                            jsonObj = jsonDoc.object();

                            jsonObj = jsonObj["SCPIInfo"].toObject();

                            scpiModuleName = jsonObj["Name"].toString();

                            if (scpiModuleName == des.m_sSCPIModuleName)
                            {
                                moduleFound = true;
                                break;
                            }
                        }
                    }
                }

                if (moduleFound)
                {
                    if (m_pModule->m_pStorageSystem->hasStoredValue(entityID, des.m_sComponentName))
                    {
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

}
