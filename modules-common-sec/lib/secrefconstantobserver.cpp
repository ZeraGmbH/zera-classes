#include "secrefconstantobserver.h"
#include "taskregisterrefconstchangenotifications.h"
#include "taskgetrefinputconstant.h"
#include <taskcontainersequence.h>
#include <cmath>

static constexpr int firstNotifcationId = 42;

void SecRefConstantObserver::registerNofifications(Zera::PcbInterfacePtr pcbInterface,
                                                   QStringList refInputNameList,
                                                   std::function<void ()> additionalErrorHandler)
{
    Q_ASSERT(!m_pcbInterface);
    m_pcbInterface = pcbInterface;
    connectPcbServerNotificationSlot();
    TaskContainerInterfacePtr initTaskSequence = TaskContainerSequence::create();
    initTaskSequence->addSub(createNotificationTasks(pcbInterface, refInputNameList, additionalErrorHandler));
    initTaskSequence->addSub(createRefConstantFetchTasks(refInputNameList, additionalErrorHandler));
    connect(initTaskSequence.get(), &TaskTemplate::sigFinish,
            this, &SecRefConstantObserver::sigRegistrationFinished);
    startTask(std::move(initTaskSequence));
}

double SecRefConstantObserver::getRefConstant(QString refChannnel)
{
    double constant = 0;
    auto iter = m_refConstants.constFind(refChannnel);
    if(iter != m_refConstants.constEnd())
        constant = *(iter.value());
    return constant;
}

void SecRefConstantObserver::onFinishKillTaskObject(bool ok, int taskId)
{
    Q_UNUSED(ok)
    m_pendingTaskIds.erase(taskId);
}

void SecRefConstantObserver::onSingleConstantFetchFinish(bool ok, int taskId)
{
    if(ok && m_pendingSingleFetchTaskIds.contains(taskId)) {
        QString refInputName = m_pendingSingleFetchTaskIds.take(taskId);
        emit sigRefConstantChanged(refInputName);
    }
}

void SecRefConstantObserver::onPcbServerReceive(quint32 msgnr, quint8 reply, QVariant answer)
{
    Q_UNUSED(reply)
    if (msgnr == 0) { // 0 was reserved for async. messages
        QString strNotifyId = answer.toString().section(':', 1, 1);
        int notifyId = strNotifyId.toInt();
        tryStartSingleConstantFetch(notifyId);
    }
}

void SecRefConstantObserver::connectPcbServerNotificationSlot()
{
    connect(m_pcbInterface.get(), &AbstractServerInterface::serverAnswer,
            this, &SecRefConstantObserver::onPcbServerReceive);
}

TaskTemplatePtr SecRefConstantObserver::createNotificationTasks(Zera::PcbInterfacePtr pcbInterface,
                                                                QStringList refInputNameList,
                                                                std::function<void ()> additionalErrorHandler)
{
    TaskRegisterRefConstChangeNotificationsPtr registerTasks = TaskRegisterRefConstChangeNotifications::create(pcbInterface,
                                                                                                               refInputNameList,
                                                                                                               firstNotifcationId,
                                                                                                               additionalErrorHandler);
    m_registerdNotifications = registerTasks->getNotificationIds();
    return registerTasks;
}

void SecRefConstantObserver::tryStartSingleConstantFetch(int notifyId)
{
    if(m_registerdNotifications.contains(notifyId)) {
        QString refInputName = m_registerdNotifications[notifyId];
        TaskTemplatePtr task = createSingleFetchTask(refInputName);
        m_pendingSingleFetchTaskIds[task->getTaskId()] = refInputName;
        connect(task.get(), &TaskTemplate::sigFinish,
                this, &SecRefConstantObserver::onSingleConstantFetchFinish);
        startTask(std::move(task));
    }
}

TaskTemplatePtr SecRefConstantObserver::createSingleFetchTask(QString refInputName,
                                                              std::function<void ()> additionalErrorHandler)
{
    return TaskGetRefInputConstant::create(m_pcbInterface,
                                           refInputName,
                                           m_refConstants[refInputName],
                                           TRANSACTION_TIMEOUT,
                                           additionalErrorHandler);
}

TaskTemplatePtr SecRefConstantObserver::createRefConstantFetchTasks(QStringList refInputNameList,
                                                                    std::function<void ()> additionalErrorHandler)
{
    TaskContainerInterfacePtr tasksInitialFetch = TaskContainerParallel::create();
    for(const auto &refInputName : qAsConst(refInputNameList)) {
        makeSureSharedPtrToRefConstIsCreated(refInputName);
        tasksInitialFetch->addSub(createSingleFetchTask(refInputName, additionalErrorHandler));
    }
    return tasksInitialFetch;
}

void SecRefConstantObserver::makeSureSharedPtrToRefConstIsCreated(QString refInputName)
{
    if(!m_refConstants.contains(refInputName)) {
        std::shared_ptr<double> refConstant = std::make_shared<double>();
        *refConstant = 0.0;
        m_refConstants[refInputName] = refConstant;
    }
}

void SecRefConstantObserver::startTask(TaskTemplatePtr task)
{
    // keep alive until finished
    int taskId = task->getTaskId();
    connect(task.get(), &TaskTemplate::sigFinish,
            this, &SecRefConstantObserver::onFinishKillTaskObject);
    m_pendingTaskIds[taskId] = std::move(task);
    m_pendingTaskIds[taskId]->start();
}
