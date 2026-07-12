#ifndef SCPISERIALCLIENT
#define SCPISERIALCLIENT

#include "scpiclient.h"
#include <QSerialPort>

namespace SCPIMODULE
{

class cSCPISerialClient: public cSCPIClient
{
    Q_OBJECT
public:
    cSCPISerialClient(QSerialPort* serial, cSCPIModule *module);
    ~cSCPISerialClient() override;

    void handleCmdFinish(const NullableString &scpiResponse, const ScpiTransactionId &scpiTransactionId, FinishLogTypes logType = LOG_FULL) override;

private slots:
    void cmdInput() override;
private:
    QSerialPort* m_pSerialPort = nullptr;
};

}
#endif // SCPISERIALCLIENT

