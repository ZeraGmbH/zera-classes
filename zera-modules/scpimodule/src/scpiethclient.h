#ifndef SCPIETHCLIENT
#define SCPIETHCLIENT

#include <QObject>

#include "scpiclient.h"

class QTcpSocket;

namespace SCPIMODULE
{

class cSCPIInterface;
class cSCPIModuleConfigData;

class cSCPIEthClient: public cSCPIClient
{
    Q_OBJECT

public:
    cSCPIEthClient(QTcpSocket* socket, VeinPeer* peer, cSCPIModuleConfigData& configdata, cSCPIInterface* iface);
    virtual ~cSCPIEthClient();

public slots:
    virtual void receiveAnswer(QString answ);

private:
    QTcpSocket* m_pSocket;

private slots:
    virtual void cmdInput();
};

}

#endif // SCPIETHCLIENT

