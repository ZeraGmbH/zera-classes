#ifndef TASKTIMEOUTDECORATOR_H
#define TASKTIMEOUTDECORATOR_H

#include "taskcomposit.h"
#include <QTimer>

class TaskTimeoutDecorator : public TaskComposite
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskTimeoutDecorator> create(TaskInterfacePtr decoratedTask, int timeout);
    TaskTimeoutDecorator(TaskInterfacePtr decoratedTask, int timeout);
    void start() override;
private slots:
    void onFinishDecorated(bool ok);
    void onTimeout();
private:
    void startDecoratedTask();
    void emitFinish(bool ok);
    TaskInterfacePtr m_decoratedTask;
    int m_timeout;
    QTimer m_timer;
};

typedef std::unique_ptr<TaskTimeoutDecorator> TaskTimeoutDecoratorPtr;

#endif // TASKTIMEOUTDECORATOR_H
