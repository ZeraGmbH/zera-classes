#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>

#include <scpi.h>
#include <veinhub.h>
#include <veinpeer.h>
#include <veinentity.h>

#include "ieee488-2.h"
#include "scpiclient.h"
#include "scpiinterface.h"
#include "scpistatus.h"
#include "scpimoduleconfigdata.h"
#include "signalconnectiondelegate.h"


namespace SCPIMODULE
{

cSCPIClient::cSCPIClient(QTcpSocket* socket, VeinPeer* peer, cSCPIModuleConfigData &configdata, cSCPIInterface* iface)
    :m_pSocket(socket), m_pPeer(peer), m_ConfigData(configdata), m_pSCPIInterface(iface)
{
    m_bAuthorisation = false;

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

    m_pIEEE4882 = new cIEEE4882(this, m_ConfigData.m_sDeviceIdentification, 50);

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

    // so now we can start our connection
    connect(m_pSocket, SIGNAL(readyRead()), this, SLOT(cmdInput()));
    connect(m_pSocket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
}


cSCPIClient::~cSCPIClient()
{
    m_pSocket->abort();
    for (int i = 0; i < m_SCPIStatusList.count(); i++)
        delete m_SCPIStatusList.at(i);
    for (int i = 0; i < sConnectDelegateList.count(); i++)
        delete sConnectDelegateList.at(i);

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


cIEEE4882 *cSCPIClient::getIEEE4882()
{
    return m_pIEEE4882;
}


void cSCPIClient::cmdInput()
{
    QString m_sInput;

    m_sInput = "";
    while ( m_pSocket->canReadLine() )
        m_sInput += m_pSocket->readLine();

    m_sInput.remove('\r'); // we remove cr lf
    m_sInput.remove('\n');

    if (!m_pSCPIInterface->executeCmd(this, m_sInput))
        emit m_pIEEE4882->AddEventError(CommandError);
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


void cSCPIClient::receiveAnswer(QString answ)
{
    QString answer;
    QByteArray ba;
    answer = answ +"\n";
    ba = answer.toLatin1();
    m_pSocket->write(ba);
}


void cSCPIClient::setSignalConnections(cSCPIStatus* scpiStatus, QList<statusBitDescriptor> &dList)
{
    if (dList.count() > 0)
    {
        QList<VeinPeer*> peerList;
        VeinHub *hub = m_pPeer->getHub(); // first we fetch a list of all our peers
        peerList = hub->listPeers();

        int n = dList.count();

        for (int i = 0; i < n; i++)
        {
            VeinPeer* peer;
            VeinEntity* entity;
            QString scpiModuleName;
            QJsonDocument jsonDoc; // we parse over all moduleinterface entities
            QJsonObject jsonObj;
            statusBitDescriptor des;
            bool found;

            found = false;
            des = dList.at(i);

            for (int j = 0; j < peerList.count(); j++)
            {
                peer = peerList.at(j);
                entity = peer->getEntityByName(QString("INF_ModuleInterface"));
                if (entity != 0) // modulemangers and interfaces do not export INF_ModuleInterface
                {
                    jsonDoc = QJsonDocument::fromBinaryData(entity->getValue().toByteArray());
                    jsonObj = jsonDoc.object();
                    scpiModuleName = jsonObj["SCPIModuleName"].toString();
                    if (scpiModuleName == des.m_sModule)
                        found = true; // we found the configured scpimodulename
                }

                if (found)
                    break;
            }

            if (found)
            {
                entity = peer->getEntityByName(des.m_sEntity); // we fetch entity by name
                if (entity != 0)
                {
                    // this is a valid signal, so let' s connect this
                    cSignalConnectionDelegate* sConnectDelegate;
                    sConnectDelegate = new cSignalConnectionDelegate(scpiStatus, des.m_nBitNr);
                    sConnectDelegateList.append(sConnectDelegate);
                    connect(entity, SIGNAL(sigValueChanged(QVariant)), sConnectDelegate, SLOT(setStatus(QVariant)));
                }
            }
        }
    }
}

}
