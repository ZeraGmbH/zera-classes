#ifndef BASEMEASWORKPROGRAM
#define BASEMEASWORKPROGRAM

#include "moduleactivist.h"
#include "basemoduleconfiguration.h"

class cBaseMeasWorkProgram: public cModuleActivist
{
    Q_OBJECT
public:
    cBaseMeasWorkProgram(std::shared_ptr<BaseModuleConfiguration> pConfiguration);
signals:
    void actualValues(QVector<float>*);
public slots:
    virtual void start() = 0; // difference between start and stop is that actual values
    virtual void stop() = 0; // in interface are not updated when stop
protected:
    QVector<float> m_ModuleActualValues; // a modules actual values
    std::shared_ptr<BaseModuleConfiguration> m_pConfiguration;
};

#endif // BASEMEASWORKPROGRAM

