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
    //void unregisterNofifications();
    double getRefConstant(QString refChannnel);
signals:
    void sigRegistrationFinished(bool ok);
    void sigRefConstantChanged(QString refInputName);

private slots:
    void onTaskFinish(bool ok, int taskId);
    void onSingleFetchTaskFinish(bool ok, int taskId);
    void onServerReceive(quint32 msgnr, quint8 reply, QVariant answer);
private:
    void startGetContants(QStringList refChannelList);
    void connectServerNotificationSlot();
    TaskTemplatePtr createNotificationTasks(Zera::PcbInterfacePtr pcbInterface,
                                            QStringList refInputNameList,
                                            std::function<void ()> additionalErrorHandler);
    TaskTemplatePtr createRefConstantFetchTasks(QStringList refInputNameList,
                                                std::function<void ()> additionalErrorHandler = []{});
    TaskTemplatePtr createSingleFetchTask(QString refInputName, std::function<void ()> additionalErrorHandler = []{});
    void startSingleFetch(int notifyId);
    void startTask(TaskTemplatePtr task);
    void initRefConstantVal(QString refInputName);

    Zera::PcbInterfacePtr m_pcbInterface;
    QHash<QString/* refInputName */, std::shared_ptr<double>> m_refConstants;
    std::unordered_map<int /*taskId*/, TaskTemplatePtr> m_pendingTasks;
    QHash<int /*taskId*/, QString /* refInputName */> m_pendingFetchTaskConstantNames;
    QMap<int /* notifyId */, QString /* refInputName */> m_registerdNotifications;
};

#endif // SECREFCONSTANTOBSERVER_H
