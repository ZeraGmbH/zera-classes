#ifndef BASEMEASWORKPROGRAM
#define BASEMEASWORKPROGRAM


#include <QObject>
#include <QStringList>
#include <QVector>
#include <QHash>
#include <memory>

#include "moduleactivist.h"
#include "basemoduleeventsystem.h"
#include "basemoduleconfiguration.h"

class cBaseMeasWorkProgram: public cModuleActivist
{
    Q_OBJECT

public:
    cBaseMeasWorkProgram(std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
    virtual ~cBaseMeasWorkProgram();
    cBaseModuleEventSystem* getEventSystem();

signals:
    void actualValues(QVector<float>*);

public slots:
    virtual void start() = 0; // difference between start and stop is that actual values
    virtual void stop() = 0; // in interface are not updated when stop

protected:
    QVector<float> m_ModuleActualValues; // a modules actual values
    cBaseModuleEventSystem *m_pEventSystem;
    std::shared_ptr<cBaseModuleConfiguration> m_pConfiguration;
};



#endif // BASEMEASWORKPROGRAM

