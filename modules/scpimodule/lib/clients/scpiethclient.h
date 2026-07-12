#ifndef SCPIETHCLIENT
#define SCPIETHCLIENT

#include "scpiclient.h"
#include <QTcpSocket>

namespace SCPIMODULE {

class cSCPIEthClient: public cSCPIClient
{
    Q_OBJECT
public:
    cSCPIEthClient(QTcpSocket* socket, cSCPIModule *module);
    ~cSCPIEthClient() override;

    void handleCmdFinish(const NullableString &scpiResponse, const ScpiTransactionId &scpiTransactionId, FinishLogTypes logType = LOG_FULL) override;
    QString getPeerAddress();

private slots:
    void cmdInput() override;
private:
    QTcpSocket* m_pSocket = nullptr;
};

}

#endif // SCPIETHCLIENT

