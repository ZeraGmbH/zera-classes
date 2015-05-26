#ifndef SCPICLIENT_H
#define SCPICLIENT_H

#include <QObject>
#include <QList>

class QTcpSocket;
class VeinPeer;

namespace SCPIMODULE
{

class cSCPIInterface;
class cSCPIStatus;
class cIEEE4882;
class cSCPIModuleConfigData;
class statusBitDescriptor;
class cSignalConnectionDelegate;

class cSCPIClient: public QObject
{
    Q_OBJECT

public:
    cSCPIClient(QTcpSocket* socket, VeinPeer* peer, cSCPIModuleConfigData& configdata, cSCPIInterface* iface);
    virtual ~cSCPIClient();

    void setAuthorisation(bool auth);
    cSCPIStatus* getSCPIStatus(quint8 index);
    cIEEE4882* getIEEE4882();

public slots:
    void receiveStatus(quint8 stat);
    void receiveAnswer(QString answ);

private:
    QTcpSocket* m_pSocket;
    VeinPeer* m_pPeer;
    cSCPIModuleConfigData& m_ConfigData;
    cSCPIInterface* m_pSCPIInterface;
    QList<cSCPIStatus*> m_SCPIStatusList;
    cIEEE4882* m_pIEEE4882;
    bool m_bAuthorisation;
    QList<cSignalConnectionDelegate*> sConnectDelegateList;

    void setSignalConnections(cSCPIStatus* scpiStatus, QList<statusBitDescriptor>& dList);

private slots:
    void cmdInput();

};

}
#endif // SCPICLIENT_H
