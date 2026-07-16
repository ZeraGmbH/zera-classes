#ifndef SCPISERVER_H
#define SCPISERVER_H

#include "scpimodelmeasure.h"
#include "scpimodelparameters.h"
#include "scpimoduleconfigdata.h"
#include "scpiclient.h"
#include "scpiserialclient.h"
#include "tcpserverlimitedconn.h"
#include <vfmoduleparameter.h>
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
    void generateVeinInterface() override;

    void addScpiClient(cSCPIClient *client);
    void removeScpiClient(cSCPIClient *client);
    const QList<cSCPIClient*> &getClients() const;
public slots:
    void deleteNetClient(SCPIMODULE::cSCPIClient *client);
private slots:
    void addNetClient();
    void TCPError(QAbstractSocket::SocketError);

    void setupTCPServer();
    void activationDone();
    void shutdownTCPServer();
    void deactivationDone();
    void testSerial();

    // vein change handlers
    void newSerialOn(const QVariant &serialOn);
private:
    void createSerialScpi();
    void destroySerialScpi();
    void deleteSerialPort();

    cSCPIModule* m_pModule;
    cSCPIModuleConfigData& m_ConfigData;
    TcpServerLimitedConn* m_pTcpServer;
    QList<cSCPIClient*> m_SCPIClientList;

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
    VfModuleComponent* m_pVeinSerialScpiDevFileName = nullptr;
};

}

#endif // SCPISERVER_H
