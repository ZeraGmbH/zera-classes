#ifndef BASEMEASWORKPROGRAM
#define BASEMEASWORKPROGRAM


#include <QObject>
#include <QStringList>
#include <QVector>
#include <QHash>

#include "moduleactivist.h"

class VeinPeer;


class cBaseMeasWorkProgram: public cModuleActivist
{
    Q_OBJECT

public:
    cBaseMeasWorkProgram(VeinPeer* peer);
    virtual ~cBaseMeasWorkProgram();
    virtual void generateInterface() = 0; // here we export our interface (entities)
    virtual void deleteInterface() = 0; // we delete interface in case of reconfiguration
    virtual void exportInterface(QJsonArray &jsArr) = 0;

signals:
    void actualValues(QVector<float>*);

public slots:
    virtual void start() = 0; // difference between start and stop is that actual values
    virtual void stop() = 0; // in interface are not updated when stop

protected:
    VeinPeer* m_pPeer; // the peer where we set our entities and get our input from
    QVector<float> m_ModuleActualValues; // a modules actual values
};



#endif // BASEMEASWORKPROGRAM

