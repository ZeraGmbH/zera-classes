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
    cSCPISerialClient(QSerialPort* serial, cSCPIModule *module, cSCPIModuleConfigData& configdata, cSCPIInterface* iface);
    ~cSCPISerialClient() override;

private slots:
    void cmdInput() override;
    void receiveAnswer(QString answ, const ScpiTransactionId &scpiTransactionId, bool ok = true, bool skipLog = false) override;
private:
    QSerialPort* m_pSerialPort = nullptr;
};

}
#endif // SCPISERIALCLIENT

