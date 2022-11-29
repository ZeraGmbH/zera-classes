#ifndef TASKCOMPOSIT_H
#define TASKCOMPOSIT_H

#include <QObject>
#include <memory>

class TaskComposite : public QObject
{
    Q_OBJECT
public:
    virtual void start() = 0;
signals:
    void finishOk();
    void finishErr();
};

typedef std::unique_ptr<TaskComposite> TaskInterfacePtr;


#endif // TASKCOMPOSIT_H
