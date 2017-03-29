#ifndef SCPISERVER_H
#define SCPISERVER_H

#include <QTimer>
#include <QTcpServer>
#include <QAbstractSocket>
#include <QHash>
#include <QState>
#include <QFinalState>

#include "moduleactivist.h"


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
class cSCPISerialClient;

class cSCPIServer: public cModuleActivist
{
    Q_OBJECT

public:
    cSCPIServer(cSCPIModule* module, cSCPIModuleConfigData& configData);
    virtual ~cSCPIServer();

    virtual void generateInterface(); // here we export our interface (entities)
    virtual void deleteInterface(); // we delete interface in case of reconfiguration
    cModuleInterface* getModuleInterface();

private:
    cSCPIModule* m_pModule;
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
    cSCPISerialClient *m_pSerialClient;
    QTimer m_SerialTestTimer;
    bool m_bSerial;

private slots:
    void addSCPIClient();
    void deleteSCPIClient(QObject* obj);
    void TCPError(QAbstractSocket::SocketError);

    void setupTCPServer();
    void activationDone();
    void shutdownTCPServer();
    void deactivationDone();
    void testSerial();
};

}

#endif // SCPISERVER_H
