#ifndef SCPISERIALCLIENT
#define SCPISERIALCLIENT


#include <QObject>

#include "scpiclient.h"

class QSerialPort;

namespace SCPIMODULE
{

class cSCPIInterface;
class cSCPIModuleConfigData;
class cSCPIModule;


class cSCPISerialClient: public cSCPIClient
{
    Q_OBJECT

public:
    cSCPISerialClient(QSerialPort* serial, cSCPIModule *module, cSCPIModuleConfigData& configdata, cSCPIInterface* iface);
    virtual ~cSCPISerialClient();

public slots:
    virtual void receiveAnswer(QString answ);

private:
    QSerialPort* m_pSerial;

private slots:
    virtual void cmdInput();
};

}
#endif // SCPISERIALCLIENT

