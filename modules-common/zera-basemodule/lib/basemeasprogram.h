#ifndef BASEMEASPROGRAM_H
#define BASEMEASPROGRAM_H

#include "moduleactivist.h"

class cBaseMeasProgram: public cModuleActivist
{
    Q_OBJECT
public:
    explicit cBaseMeasProgram(const QString &moduleName);
signals:
    void actualValues(QVector<float>*);
public slots:
    virtual void start() = 0; // difference between start and stop is that actual values
    virtual void stop() = 0; // in interface are not updated when stop
    void monitorConnection();

protected:
    quint8 m_nConnectionCount = 0;

    QVector<float> m_ModuleActualValues; // a modules actual values
    QHash<quint32, int> m_MsgNrCmdList;
};


#endif // BASEMEASPROGRAM_H
