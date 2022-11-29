#ifndef TASKTIMEOUTDECORATOR_H
#define TASKTIMEOUTDECORATOR_H

#include "taskcomposit.h"

class TaskTimeoutDecorator : public TaskComposite
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskTimeoutDecorator> create(TaskInterfacePtr decoratedTask, int timeout);
    TaskTimeoutDecorator(TaskInterfacePtr decoratedTask, int timeout);
    void start() override;
private slots:
    void onFinishDecorated(bool ok);
private:
    TaskInterfacePtr m_decoratedTask;
    int m_timeout;

};

typedef std::unique_ptr<TaskTimeoutDecorator> TaskTimeoutDecoratorPtr;

#endif // TASKTIMEOUTDECORATOR_H
