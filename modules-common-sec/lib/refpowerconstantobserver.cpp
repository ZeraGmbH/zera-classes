#include "refpowerconstantobserver.h"
#include "taskrefpowerregisterconstchangenotifications.h"
#include "taskrefpowerfetchconstant.h"
#include <taskcontainersequence.h>
#include <cmath>

static constexpr int firstNotifcationId = 42;

void RefPowerConstantObserver::registerNofifications(Zera::PcbInterfacePtr pcbInterface,
                                                     QStringList refPowerNameList,
                                                     std::function<void ()> additionalErrorHandler)
{
    Q_ASSERT(!m_pcbInterface);
    m_pcbInterface = pcbInterface;
    connectPcbServerNotificationSlot();
    TaskContainerInterfacePtr initTaskSequence = TaskContainerSequence::create();
    initTaskSequence->addSub(createNotificationTasks(pcbInterface, refPowerNameList, additionalErrorHandler));
    initTaskSequence->addSub(createRefConstantFetchTasks(refPowerNameList, additionalErrorHandler));
    connect(initTaskSequence.get(), &TaskTemplate::sigFinish,
            this, &RefPowerConstantObserver::sigRegistrationFinished);
    startTask(std::move(initTaskSequence));
}

double RefPowerConstantObserver::getConstant(QString refPowerName)
{
    double constant = 0;
    auto iter = m_refPowerConstants.constFind(refPowerName);
    if(iter != m_refPowerConstants.constEnd())
        constant = *(iter.value());
    return constant;
}

void RefPowerConstantObserver::onFinishKillTaskObject(bool ok, int taskId)
{
    Q_UNUSED(ok)
    m_pendingTaskIds.erase(taskId);
}

void RefPowerConstantObserver::onSingleConstantFetchFinish(bool ok, int taskId)
{
    if(ok) {
        if(m_pendingSingleFetchTaskIds.contains(taskId)) {
            QString refPowerName = m_pendingSingleFetchTaskIds.take(taskId);
            emit sigRefConstantChanged(refPowerName);
        }
    }
    else
        // For now just warn. If an issue, another arror handler can be added
        qWarning("SecRefConstantObserver::onSingleConstantFetchFinish reported an error!");
}

void RefPowerConstantObserver::onPcbServerReceive(quint32 msgnr, quint8 reply, QVariant answer)
{
    Q_UNUSED(reply)
    if (msgnr == 0) { // 0 was reserved for async. messages
        QString strNotifyId = answer.toString().section(':', 1, 1);
        int notifyId = strNotifyId.toInt();
        tryStartSingleConstantFetch(notifyId);
    }
}

void RefPowerConstantObserver::connectPcbServerNotificationSlot()
{
    connect(m_pcbInterface.get(), &AbstractServerInterface::serverAnswer,
            this, &RefPowerConstantObserver::onPcbServerReceive);
}

TaskTemplatePtr RefPowerConstantObserver::createNotificationTasks(Zera::PcbInterfacePtr pcbInterface,
                                                                  QStringList refPowerNameList,
                                                                  std::function<void ()> additionalErrorHandler)
{
    TaskRegisterRefConstChangeNotificationsPtr registerTasks =
        TaskRefPowerRegisterConstChangeNotifications::create(pcbInterface,
                                                             refPowerNameList,
                                                             firstNotifcationId,
                                                             additionalErrorHandler);
    m_registerdNotifications = registerTasks->getNotificationIds();
    return registerTasks;
}

void RefPowerConstantObserver::tryStartSingleConstantFetch(int notifyId)
{
    if(m_registerdNotifications.contains(notifyId)) {
        QString refPowerName = m_registerdNotifications[notifyId];
        TaskTemplatePtr task = createSingleFetchTask(refPowerName);
        m_pendingSingleFetchTaskIds[task->getTaskId()] = refPowerName;
        connect(task.get(), &TaskTemplate::sigFinish,
                this, &RefPowerConstantObserver::onSingleConstantFetchFinish);
        startTask(std::move(task));
    }
}

TaskTemplatePtr RefPowerConstantObserver::createSingleFetchTask(QString refPowerName,
                                                                std::function<void ()> additionalErrorHandler)
{
    return TaskRefPowerFetchConstant::create(m_pcbInterface,
                                             refPowerName,
                                             m_refPowerConstants[refPowerName],
                                             TRANSACTION_TIMEOUT,
                                             additionalErrorHandler);
}

TaskTemplatePtr RefPowerConstantObserver::createRefConstantFetchTasks(QStringList refPowerNameList,
                                                                      std::function<void ()> additionalErrorHandler)
{
    TaskContainerInterfacePtr tasksInitialFetch = TaskContainerParallel::create();
    for(const auto &refPowerName : qAsConst(refPowerNameList)) {
        makeSureSharedPtrToRefPowerConstIsCreated(refPowerName);
        tasksInitialFetch->addSub(createSingleFetchTask(refPowerName, additionalErrorHandler));
    }
    return tasksInitialFetch;
}

void RefPowerConstantObserver::makeSureSharedPtrToRefPowerConstIsCreated(QString refPowerName)
{
    if(!m_refPowerConstants.contains(refPowerName)) {
        std::shared_ptr<double> refConstant = std::make_shared<double>();
        *refConstant = 0.0;
        m_refPowerConstants[refPowerName] = refConstant;
    }
}

void RefPowerConstantObserver::startTask(TaskTemplatePtr task)
{
    // keep alive until finished
    int taskId = task->getTaskId();
    connect(task.get(), &TaskTemplate::sigFinish,
            this, &RefPowerConstantObserver::onFinishKillTaskObject);
    m_pendingTaskIds[taskId] = std::move(task);
    m_pendingTaskIds[taskId]->start();
}
