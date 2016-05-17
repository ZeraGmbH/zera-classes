#ifndef BASEMEASWORKPROGRAM
#define BASEMEASWORKPROGRAM


#include <QObject>
#include <QStringList>
#include <QVector>
#include <QHash>

#include "moduleactivist.h"
#include "basemoduleeventsystem.h"

class cBaseMeasWorkProgram: public cModuleActivist
{
    Q_OBJECT

public:
    cBaseMeasWorkProgram();
    virtual ~cBaseMeasWorkProgram();
    virtual void generateInterface() = 0; // here we export our interface (entities)
    virtual void deleteInterface() = 0; // we delete interface in case of reconfiguration
    cBaseModuleEventSystem* getEventSystem();

signals:
    void actualValues(QVector<float>*);

public slots:
    virtual void start() = 0; // difference between start and stop is that actual values
    virtual void stop() = 0; // in interface are not updated when stop

protected:
    QVector<float> m_ModuleActualValues; // a modules actual values
    cBaseModuleEventSystem *m_pEventSystem;
};



#endif // BASEMEASWORKPROGRAM

