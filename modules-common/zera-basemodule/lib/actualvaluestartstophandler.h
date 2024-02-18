#ifndef ACTUALVALUESTARTSTOPHANDLER_H
#define ACTUALVALUESTARTSTOPHANDLER_H

#include <QObject>

class ActualValueStartStopHandler : public QObject
{
    Q_OBJECT
public:
    void start();
    void stop();
public slots:
    void onNewActualValues(QVector<float> *values);
signals:
    void sigNewActualValues(QVector<float>* values);
private:
    bool m_started = false;
};

#endif // ACTUALVALUESTARTSTOPHANDLER_H
