#ifndef TASKCOMPOSIT_H
#define TASKCOMPOSIT_H

#include <QObject>

class TaskComposite : public QObject
{
    Q_OBJECT
signals:
    void finishOk();
    void finishErr();
public:
    virtual void start() = 0;
};

#endif // TASKCOMPOSIT_H
