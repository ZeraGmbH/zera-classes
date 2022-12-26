#ifndef TASKTIMEOUTDECORATOR_H
#define TASKTIMEOUTDECORATOR_H

#include "taskcomposit.h"
#include <zeratimertemplate.h>
#include <memory>

class TaskTimeoutDecorator : public TaskComposite
{
    Q_OBJECT
public:
    static TaskCompositePtr wrapTimeout(int timeout, TaskCompositePtr decoratedTask,
                                        std::function<void()> additionalErrorHandler = []{});
    TaskTimeoutDecorator(ZeraTimerTemplatePtr timer, TaskCompositePtr decoratedTask);
    void start() override;
private slots:
    void onFinishDecorated(bool ok);
    void onTimeout();
private:
    void startDecoratedTask();
    void emitFinish(bool ok);
    TaskCompositePtr m_decoratedTask;
    std::unique_ptr<ZeraTimerTemplate> m_timer;
};

#endif // TASKTIMEOUTDECORATOR_H
