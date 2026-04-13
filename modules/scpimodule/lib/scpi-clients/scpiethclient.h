#ifndef SCPIETHCLIENT
#define SCPIETHCLIENT

#include "scpiclient.h"
#include "scpiinterface.h"
#include "scpimoduleconfigdata.h"
#include <QTcpSocket>

namespace SCPIMODULE {

class cSCPIEthClient: public cSCPIClient
{
    Q_OBJECT
public:
    cSCPIEthClient(QTcpSocket* socket,  cSCPIModule *module, cSCPIModuleConfigData& configdata, cSCPIInterface* iface);
    ~cSCPIEthClient() override;
    QString getPeerAddress();
private slots:
    void receiveAnswer(QString answ, bool ok = true) override;
    void cmdInput() override;
private:
    QTcpSocket* m_pSocket = nullptr;
};

}

#endif // SCPIETHCLIENT

