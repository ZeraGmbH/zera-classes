#ifndef TASKEXTRAERRORHANDLER_H
#define TASKEXTRAERRORHANDLER_H

#include "taskcomposit.h"

class TaskExtraErrorHandler : public TaskComposite
{
    Q_OBJECT
public:
    static TaskCompositePtr create(TaskCompositePtr decoratedTask, std::function<void()> additionalErrorHandler);
    TaskExtraErrorHandler(TaskCompositePtr decoratedTask, std::function<void()> additionalErrorHandler);
    void start() override;
private slots:
    void onFinishDecorated(bool ok);
private:
    void startDecoratedTask();
    void emitFinish(bool ok);
    TaskCompositePtr m_decoratedTask;
    std::function<void()> m_additionalErrorHandler;
};

#endif // TASKEXTRAERRORHANDLER_H
