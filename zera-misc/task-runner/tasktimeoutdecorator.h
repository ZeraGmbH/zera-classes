#ifndef TASKTIMEOUTDECORATOR_H
#define TASKTIMEOUTDECORATOR_H

#include "taskcomposit.h"
#include <QTimer>

class TaskTimeoutDecorator : public TaskComposite
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskTimeoutDecorator> wrapTimeout(int timeout, TaskCompositePtr decoratedTask, std::function<void()> additionalErrorHandler = []{});
    TaskTimeoutDecorator(TaskCompositePtr decoratedTask, int timeout, std::function<void()> additionalErrorHandler = []{});
    void start() override;
private slots:
    void onFinishDecorated(bool ok);
    void onTimeout();
private:
    void startDecoratedTask();
    void emitFinish(bool ok);
    TaskCompositePtr m_decoratedTask;
    int m_timeoutMs;
    QTimer m_timer;
};

typedef std::unique_ptr<TaskTimeoutDecorator> TaskTimeoutDecoratorPtr;

#endif // TASKTIMEOUTDECORATOR_H
