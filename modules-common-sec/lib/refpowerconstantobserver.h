#ifndef REFPOWERCONSTANTOBSERVER_H
#define REFPOWERCONSTANTOBSERVER_H

#include <pcbinterface.h>
#include <tasktemplate.h>
#include <unordered_map>

class RefPowerConstantObserver : public QObject
{
    Q_OBJECT
public:
    void registerNofifications(Zera::PcbInterfacePtr pcbInterface,
                               QStringList refPowerNameList,
                               std::function<void ()> additionalErrorHandler = []{});
    // unregister is done once per module. So it is not upon us.
    double getConstant(QString refChannnel);
signals:
    void sigRegistrationFinished(bool ok);
    void sigRefConstantChanged(QString refPowerName);

private slots:
    void onFinishKillTaskObject(bool ok, int taskId);
    void onSingleConstantFetchFinish(bool ok, int taskId);
    void onPcbServerReceive(quint32 msgnr, quint8 reply, QVariant answer);
private:
    void connectPcbServerNotificationSlot();
    TaskTemplatePtr createNotificationTasks(Zera::PcbInterfacePtr pcbInterface,
                                            QStringList refPowerNameList,
                                            std::function<void ()> additionalErrorHandler);
    TaskTemplatePtr createRefConstantFetchTasks(QStringList refPowerNameList,
                                                std::function<void ()> additionalErrorHandler = []{});
    TaskTemplatePtr createSingleFetchTask(QString refPowerName, std::function<void ()> additionalErrorHandler = []{});
    void tryStartSingleConstantFetch(int notifyId);
    void startTask(TaskTemplatePtr task);
    void makeSureSharedPtrToRefPowerConstIsCreated(QString refPowerName);

    Zera::PcbInterfacePtr m_pcbInterface;
    QHash<QString/* refPowerName */, std::shared_ptr<double>> m_refPowerConstants;
    std::unordered_map<int /*taskId*/, TaskTemplatePtr> m_pendingTaskIds;
    QHash<int /*taskId*/, QString /* refPowerName */> m_pendingSingleFetchTaskIds;
    QMap<int /* notifyId */, QString /* refPowerName */> m_registerdNotifications;
};

#endif // REFPOWERCONSTANTOBSERVER_H
