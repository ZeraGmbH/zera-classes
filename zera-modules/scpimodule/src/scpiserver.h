#ifndef SCPISERVER_H
#define SCPISERVER_H

#include <QTcpServer>
#include <QAbstractSocket>
#include <QHash>
#include <QState>
#include <QFinalState>

#include "moduleactivist.h"


class VeinPeer;
class QSerialPort;

namespace SCPIMODULE
{

class cSCPIModule;
class cSCPIModuleConfigData;
class cSCPIInterface;
class cModuleInterface;
class cInterfaceInterface;
class cStatusInterface;
class cIEEE4882Interface;
class cSCPIClient;

class cSCPIServer: public cModuleActivist
{
    Q_OBJECT

public:
    cSCPIServer(cSCPIModule* module, VeinPeer* peer, cSCPIModuleConfigData& configData);
    virtual ~cSCPIServer();

    virtual void generateInterface(); // here we export our interface (entities)
    virtual void deleteInterface(); // we delete interface in case of reconfiguration
    virtual void exportInterface(QJsonArray& );

private:
    cSCPIModule* m_pModule;
    VeinPeer* m_pPeer;
    cSCPIModuleConfigData& m_ConfigData;

    QTcpServer* m_pTcpServer;
    cSCPIInterface* m_pSCPIInterface;
    QList<cSCPIClient*> m_SCPIClientList;

    cModuleInterface* m_pModuleInterface;
    cInterfaceInterface* m_pInterfaceInterface;
    cStatusInterface* m_pStatusInterface;
    cIEEE4882Interface* m_pIEEE488Interface;

    // statemachine for activating gets the following states
    QState m_setupTCPServerState;
    QFinalState m_activationDoneState;

    // statemachine for activating gets the following states
    QState m_shutdownTCPServerState;
    QFinalState m_deactivationDoneState;

    // optionally we support a serial device
    QSerialPort* m_pSerial;

private slots:
    void addSCPIClient();
    void deleteSCPIClient(QObject* obj);
    void TCPError(QAbstractSocket::SocketError);

    void setupTCPServer();
    void activationDone();
    void shutdownTCPServer();
    void deactivationDone();
};

}

#endif // SCPISERVER_H
