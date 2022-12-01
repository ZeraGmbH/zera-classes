#ifndef TASKCOMPOSIT_H
#define TASKCOMPOSIT_H

#include <QObject>
#include <memory>

class TaskComposite : public QObject
{
    Q_OBJECT
public:
    virtual int start() = 0;
    virtual ~TaskComposite() = default;
signals:
    void sigFinish(bool ok, int taskId);
protected:
    static int getNextTaskId();
private:
    static int m_currTaskId;
};

typedef std::unique_ptr<TaskComposite> TaskInterfacePtr;


#endif // TASKCOMPOSIT_H
