#ifndef SECREFCONSTANTOBSERVER_H
#define SECREFCONSTANTOBSERVER_H

#include <pcbinterface.h>
#include <tasktemplate.h>
#include <unordered_map>

class SecRefConstantObserver : public QObject
{
    Q_OBJECT
public:
    void registerNofifications(Zera::PcbInterfacePtr pcbInterface,
                               QStringList refInputNameList,
                               std::function<void ()> additionalErrorHandler = []{});
    // unregister is done once per module. So it is not upon us.
    double getRefConstant(QString refChannnel);
signals:
    void sigRegistrationFinished(bool ok);
    void sigRefConstantChanged(QString refInputName);

private slots:
    void onFinishKillTaskObject(bool ok, int taskId);
    void onSingleConstantFetchFinish(bool ok, int taskId);
    void onPcbServerReceive(quint32 msgnr, quint8 reply, QVariant answer);
private:
    void connectPcbServerNotificationSlot();
    TaskTemplatePtr createNotificationTasks(Zera::PcbInterfacePtr pcbInterface,
                                            QStringList refInputNameList,
                                            std::function<void ()> additionalErrorHandler);
    TaskTemplatePtr createRefConstantFetchTasks(QStringList refInputNameList,
                                                std::function<void ()> additionalErrorHandler = []{});
    TaskTemplatePtr createSingleFetchTask(QString refInputName, std::function<void ()> additionalErrorHandler = []{});
    void tryStartSingleConstantFetch(int notifyId);
    void startTask(TaskTemplatePtr task);
    void makeSureSharedPtrToRefConstIsCreated(QString refInputName);

    Zera::PcbInterfacePtr m_pcbInterface;
    QHash<QString/* refInputName */, std::shared_ptr<double>> m_refConstants;
    std::unordered_map<int /*taskId*/, TaskTemplatePtr> m_pendingTaskIds;
    QHash<int /*taskId*/, QString /* refInputName */> m_pendingSingleFetchTaskIds;
    QMap<int /* notifyId */, QString /* refInputName */> m_registerdNotifications;
};

#endif // SECREFCONSTANTOBSERVER_H
