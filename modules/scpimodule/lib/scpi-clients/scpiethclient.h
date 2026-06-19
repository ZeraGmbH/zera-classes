#ifndef SCPIETHCLIENT
#define SCPIETHCLIENT

#include "scpiclient.h"
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
    void handleCmdFinish(QString answ, const ScpiTransactionId &scpiTransactionId, bool ok = true, bool skipLog = false) override;
    void cmdInput() override;
private:
    QTcpSocket* m_pSocket = nullptr;
};

}

#endif // SCPIETHCLIENT

