#ifndef BASEMEASPROGRAM_H
#define BASEMEASPROGRAM_H

#include <QObject>
#include <QStringList>
#include <QVector>
#include <QHash>

#include "moduleactivist.h"

class cBaseModule;
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


class cBaseMeasProgram: public cModuleActivist
{
    Q_OBJECT

public:
    cBaseMeasProgram(Zera::Proxy::cProxy* proxy);
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
    Zera::Server::cRMInterface* m_pRMInterface;
    Zera::Proxy::cProxyClient* m_pRMClient;
    // we hold an interface for every channel because it could be possible that our measuring
    // channels are spread over several pcb's
    QList<Zera::Server::cPCBInterface*> m_pcbIFaceList; // our interface(s) to pcb
    // so we must also keep a list of pcb client's we can
    QList<Zera::Proxy::cProxyClient*> m_pcbClientList; // our clients for pcb interfaces

    QVector<float> m_ModuleActualValues; // a modules actual values
    QHash<quint32, int> m_MsgNrCmdList;
    quint8 m_nConnectionCount;

protected slots:
    void monitorConnection();
};


#endif // BASEMEASPROGRAM_H
