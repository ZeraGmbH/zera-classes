#ifndef PRIORITYARBITRATION_H
#define PRIORITYARBITRATION_H

#include <ve_eventsystem.h>
#include "modman_util.h"
#include <QMap>
#include <QUuid>

class PriorityArbitrationSystem : public VeinEvent::EventSystem
{
    Q_OBJECT
public:
    enum RPCResultCodes {
        PAS_EINVAL = -EINVAL, //invalid parameters
        PAS_SUCCESS = 0,
        PAS_UNAVAILABLE = 1 //priority level not available (e.g. because it is already set)
    };

    explicit PriorityArbitrationSystem(QObject *t_parent = nullptr);

signals:

public slots:

    // EventSystem interface
public:
    bool processEvent(QEvent *t_event) override;

private:
    void initECS();
    void rpcFinished(QUuid t_callId, const QString &t_procedureName, const QVariantMap &t_data);

    VF_COMPONENT(entityName, "EntityName", "Entity name")
    static constexpr QLatin1String s_entityName = modman_util::to_latin1("_ArbitrationSystem");

    VF_COMPONENT(clientPriorityList, "ClientPriorityList", "List of clients with descending priority order")

    VF_RPC(arbitrationAdd, "arbitrationAdd(QUuid uid, int priority)", "uid: peer uid, priority: 0(highest) <= priority <= INT32_MAX(lowest)")
    void arbitrationAdd(const QUuid &t_callId, const QVariantMap &t_parameters);

    VF_RPC(arbitrationRemove, "arbitrationRemove(QUuid uid)", "uid: previously added peer uid")
    void arbitrationRemove(const QUuid &t_callId, const QVariantMap &t_parameters);

    static constexpr int s_entityId = 201;

    //functions need an instance so no static variable
    const VeinEvent::RoutedRemoteProcedureAtlas m_remoteProcedures;

    /**
   * @brief call id, peer id
   */
    QHash<QUuid, QUuid> m_pendingRpcHash;
    QMap<int, QUuid> m_priorityArbitrationMap;
};

#endif // PRIORITYARBITRATION_H
