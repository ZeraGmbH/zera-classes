#ifndef SCPISERVER_H
#define SCPISERVER_H

#include "moduleinterface.h"
#include "scpimoduleconfigdata.h"
#include "scpiclient.h"
#include "interfaceinterface.h"
#include "statusinterface.h"
#include "ieee4882interface.h"
#include "scpiserialclient.h"
#include "tcpserverlimitedconn.h"
#include <vfmoduleparameter.h>
#include <vfmoduleactvalue.h>
#include <QTimer>
#include <QFinalState>
#include <QFile>
#include <QTcpServer>
#include <QSerialPort>
#include <moduleactivist.h>

namespace SCPIMODULE
{
class cSCPIModule;

class cSCPIServer: public cModuleActivist
{
    Q_OBJECT
public:
    cSCPIServer(cSCPIModule* module, cSCPIModuleConfigData& configData);
    virtual ~cSCPIServer();
    virtual void generateInterface() override; // here we export our interface (entities)
    cModuleInterface* getModuleInterface();
    void appendClient(cSCPIClient *client);
    cSCPIInterface* getScpiInterface() const;
private slots:
    void addSCPIClient();
    void deleteSCPIClient(cSCPIClient *client);
    void TCPError(QAbstractSocket::SocketError);

    void setupTCPServer();
    void activationDone();
    void shutdownTCPServer();
    void deactivationDone();
    void testSerial();

    // vein change handlers
    void newSerialOn(QVariant serialOn);
    void controlScpiQueue(QVariant scpiQueue);
private:
    void createSerialScpi();
    void destroySerialScpi();
    void deleteSerialPort();

    cSCPIModule* m_pModule;
    cSCPIModuleConfigData& m_ConfigData;
    TcpServerLimitedConn* m_pTcpServer;
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
    QSerialPort* m_pSerialPort = nullptr;
    cSCPISerialClient *m_pSerialClient = nullptr;
    QTimer m_SerialTestTimer;
    bool m_bSerialScpiActive;
    VfModuleParameter* m_pVeinParamSerialOn = nullptr;
    VfModuleActvalue* m_pVeinSerialScpiDevFileName = nullptr;
    VfModuleActvalue* m_veinDevIface = nullptr;
    VfModuleParameter* m_optionalScpiQueue = nullptr;

};

}

#endif // SCPISERVER_H
