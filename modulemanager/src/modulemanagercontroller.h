#ifndef MODULEMANAGERSETTINGS_H
#define MODULEMANAGERSETTINGS_H


#include "modman_util.h"

#include <ve_eventsystem.h>

#include <QJsonDocument>

namespace VeinEvent {
class StorageSystem;
}

class ModuleManagerController : public VeinEvent::EventSystem
{
    Q_OBJECT
public:
    explicit ModuleManagerController(QObject *t_parent = nullptr);
    static constexpr int getEntityId();
    VeinEvent::StorageSystem *getStorageSystem() const;
    /**
   * @brief sets the storage system that is used to get the list of entity ids that the storage holds
   * @param t_storageSystem
   */
    void setStorage(VeinEvent::StorageSystem *t_storageSystem);
    static constexpr QLatin1String s_notificationMessagesComponentName = modman_util::to_latin1("Error_Messages");

    // EventSystem interface
public:
    bool processEvent(QEvent *t_event) override;

signals:
    /**
   * @brief emitted if the session is switched via the "Session" component
   * @param t_newSession
   */
    void sigChangeSession(const QString &t_newSession);
    /**
   * @brief emitted if the modules are paused via the "ModulesPaused" component
   * @param t_paused
   */
    void sigModulesPausedChanged(bool t_paused);

public slots:
    /**
   * @brief connected to the signal ModuleManager::sigModulesLoaded, loads the list of entities from the m_storageSystem then sets up the data of all components
   * @param t_sessionPath
   * @param t_sessionList
   */
    void initializeEntity(const QString &t_sessionPath, const QStringList &t_sessionList);
    /**
   * @brief sends add events for the entity and all components
   */
    void initOnce();
    /**
   * @brief emits sigModulesPausedChanged
   * @param t_paused
   */
    void setModulesPaused(bool t_paused);

private:
    /**
   * @brief the old fashioned way of writing error messages
   * @deprecated errors and warnings now are communicated via the system logging facility (probably systemd journal)
   * @param t_message
   */
    void handleNotificationMessage(QJsonObject t_message);

    static constexpr int s_entityId = 0;
    static constexpr QLatin1String s_entityName = modman_util::to_latin1("_System");
    static constexpr QLatin1String s_entityNameComponentName = modman_util::to_latin1("EntityName");
    static constexpr QLatin1String s_entitiesComponentName = modman_util::to_latin1("Entities");
    static constexpr QLatin1String s_sessionComponentName = modman_util::to_latin1("Session");
    static constexpr QLatin1String s_sessionsAvailableComponentName = modman_util::to_latin1("SessionsAvailable");
    static constexpr QLatin1String s_loggedComponentsComponentName = modman_util::to_latin1("LoggedComponents");
    static constexpr QLatin1String s_modulesPausedComponentName = modman_util::to_latin1("ModulesPaused");
    static constexpr QLatin1String s_serverIpComponentName = modman_util::to_latin1("ServerAddressList");


    VeinEvent::StorageSystem *m_storageSystem = nullptr;
    QJsonDocument m_notificationMessages;
    QString m_currentSession;
    QStringList m_availableSessions;
    bool m_initDone=false;
    bool m_sessionReady=false;
    bool m_modulesPaused=false;
};

#endif // MODULEMANAGERSETTINGS_H
