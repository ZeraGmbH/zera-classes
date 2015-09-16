#ifndef SCPICLIENT_H
#define SCPICLIENT_H

#include <QObject>
#include <QList>

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
    cSCPIClient(VeinPeer* peer, cSCPIModuleConfigData& configdata, cSCPIInterface* iface);
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
    VeinPeer* m_pPeer;
    cSCPIModuleConfigData& m_ConfigData;

    QList<cSCPIStatus*> m_SCPIStatusList;

    bool m_bAuthorisation;
    QList<cSignalConnectionDelegate*> sConnectDelegateList;

    void setSignalConnections(cSCPIStatus* scpiStatus, QList<statusBitDescriptor>& dList);

private slots:
    virtual void cmdInput() = 0;

};

}
#endif // SCPICLIENT_H
