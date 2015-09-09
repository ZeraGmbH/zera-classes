#ifndef SCPICLIENT_H
#define SCPICLIENT_H

#include <QObject>
#include <QList>


class QTcpSocket;


namespace SCPIMODULE
{

class cSCPIModule;
class cSCPIInterface;
class cSCPIStatus;
class cIEEE4882;
class cSCPIModuleConfigData;
class cStatusBitDescriptor;
class cStatusBitSignal;

class cSCPIClient: public QObject
{
    Q_OBJECT

public:
    cSCPIClient(QTcpSocket* socket, cSCPIModule* module, cSCPIModuleConfigData& configdata, cSCPIInterface* iface);
    virtual ~cSCPIClient();

    void setAuthorisation(bool auth);
    cSCPIStatus* getSCPIStatus(quint8 index);
    cIEEE4882* getIEEE4882();

public slots:
    void receiveStatus(quint8 stat);
    virtual void receiveAnswer(QString answ) = 0;

protected:
    cSCPIInterface* m_pSCPIInterface;
    cIEEE4882* m_pIEEE4882;

private:
    QTcpSocket* m_pSocket;
    cSCPIModule* m_pModule;
    cSCPIModuleConfigData& m_ConfigData;

    QList<cSCPIStatus*> m_SCPIStatusList;
    QList<cStatusBitSignal*> mStatusBitSignalList;

    cIEEE4882* m_pIEEE4882;

    bool m_bAuthorisation;
    void setSignalConnections(cSCPIStatus* scpiStatus, QList<cStatusBitDescriptor> &dList);

private slots:
    virtual void cmdInput() = 0;

};

}
#endif // SCPICLIENT_H
