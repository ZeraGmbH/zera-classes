#ifndef BASEMEASPROGRAM_H
#define BASEMEASPROGRAM_H

#include "basemoduleconfiguration.h"
#include "basemodule.h"
#include "socket.h"
#include "moduleactivist.h"
#include <pcbinterface.h>
#include <rminterface.h>

class cBaseMeasProgram: public cModuleActivist
{
    Q_OBJECT
public:
    cBaseMeasProgram(std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
signals:
    void actualValues(QVector<float>*);
public slots:
    virtual void start() = 0; // difference between start and stop is that actual values
    virtual void stop() = 0; // in interface are not updated when stop
    void monitorConnection();

protected:
    std::shared_ptr<cBaseModuleConfiguration> m_pConfiguration;
    Zera::cRMInterface m_rmInterface;
    Zera::ProxyClientPtr m_rmClient;
    // we hold an interface for every channel because it could be possible that our measuring
    // channels are spread over several pcb's
    QList<Zera::cPCBInterface*> m_pcbIFaceList; // our interface(s) to pcb
    // so we must also keep a list of pcb client's we can
    QList<Zera::ProxyClient*> m_pcbClientList; // our clients for pcb interfaces

    QVector<float> m_ModuleActualValues; // a modules actual values
    QHash<quint32, int> m_MsgNrCmdList;
    quint8 m_nConnectionCount;
};


#endif // BASEMEASPROGRAM_H
