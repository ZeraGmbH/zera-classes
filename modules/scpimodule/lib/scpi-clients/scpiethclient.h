#ifndef SCPIETHCLIENT
#define SCPIETHCLIENT

#include "scpiclient.h"
#include "scpiinterface.h"
#include "scpimodule.h"
#include "scpimoduleconfigdata.h"
#include <QObject>
#include <QTcpSocket>

namespace SCPIMODULE {

class cSCPIEthClient: public cSCPIClient
{
    Q_OBJECT
public:
    cSCPIEthClient(QTcpSocket* socket,  cSCPIModule *module, cSCPIModuleConfigData& configdata, cSCPIInterface* iface);
    virtual ~cSCPIEthClient();
private slots:
    void receiveAnswer(QString answ, bool ok = true) override;
    void cmdInput() override;
    void onDisconnect();
private:
    QString getPeerAddress();
    QString m_peerAddress;
    QTcpSocket* m_pSocket;
};

}

#endif // SCPIETHCLIENT

