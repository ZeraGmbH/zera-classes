#ifndef BASEMEASPROGRAM_H
#define BASEMEASPROGRAM_H

#include "basemoduleconfiguration.h"
#include "moduleactivist.h"
#include "basemodule.h"
#include <rminterface.h>

class cBaseMeasProgram: public cModuleActivist
{
    Q_OBJECT
public:
    cBaseMeasProgram(std::shared_ptr<BaseModuleConfiguration> pConfiguration, QString moduleName);
signals:
    void actualValues(QVector<float>*);
public slots:
    virtual void start() = 0; // difference between start and stop is that actual values
    virtual void stop() = 0; // in interface are not updated when stop
    void monitorConnection();

protected:
    std::shared_ptr<BaseModuleConfiguration> m_pConfiguration;
    Zera::cRMInterface m_rmInterface;
    Zera::ProxyClientPtr m_rmClient;

    quint8 m_nConnectionCount;

    QVector<float> m_ModuleActualValues; // a modules actual values
    QHash<quint32, int> m_MsgNrCmdList;
};


#endif // BASEMEASPROGRAM_H
