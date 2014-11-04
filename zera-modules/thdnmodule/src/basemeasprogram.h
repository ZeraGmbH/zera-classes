#ifndef BASEMEASPROGRAM_H
#define BASEMEASPROGRAM_H

#include <QObject>
#include <QStringList>
#include <QVector>
#include <QHash>

#include "moduleactivist.h"

class cBaseModule;
class VeinPeer;
class cSocket;


namespace Zera {
namespace Proxy {
    class cProxy;
    class cProxyClient;
}
namespace  Server {
    class cRMInterface;
    class cDSPInterface;
    class cPCBInterface;
}
}

namespace THDNMODULE {

class cBaseMeasProgram: public cModuleActivist
{
    Q_OBJECT

public:
    cBaseMeasProgram(Zera::Proxy::cProxy* proxy, VeinPeer* peer);
    virtual ~cBaseMeasProgram();
    virtual void generateInterface() = 0; // here we export our interface (entities)
    virtual void deleteInterface() = 0; // we delete interface in case of reconfiguration

signals:
    void actualValues(QVector<float>*);

public slots:
    virtual void start() = 0; // difference between start and stop is that actual values
    virtual void stop() = 0; // in interface are not updated when stop

protected:
    Zera::Proxy::cProxy* m_pProxy; // the proxy where we can get our connections
    VeinPeer* m_pPeer; // the peer where we set our entities
    // the module creates a central dsp interface and forwards this
    // the reason behind this is, that dsp server generates a new "environment" for each
    // new client. but we want to access the same environment's value from all over the module.
    Zera::Server::cDSPInterface* m_pDSPInterFace; // our interface to dsp
    Zera::Proxy::cProxyClient *m_pDspClient;
    Zera::Server::cRMInterface* m_pRMInterface;
    Zera::Proxy::cProxyClient* m_pRMClient;
    // we hold an interface for every channel because it could be possible that our measuring
    // channels are spread over several pcb's
    QList<Zera::Server::cPCBInterface*> m_pcbIFaceList; // our interface(s) to pcb
    // so we must also keep a list of pcb client's we can
    QList<Zera::Proxy::cProxyClient*> m_pcbClientList; // our clients for pcb interfaces

    QVector<float> m_ModuleActualValues; // a modules actual values
    QHash<quint32, int> m_MsgNrCmdList;
    quint32 m_nDspMemUsed;

    virtual void setDspVarList() = 0; // dsp related stuff
    virtual void deleteDspVarList() = 0;
    virtual void setDspCmdList() = 0;
    virtual void deleteDspCmdList() = 0;

    quint8 m_nConnectionCount;

protected slots:
    void monitorConnection();
};

}

#endif // BASEMEASPROGRAM_H
