#ifndef CLIENTACTIVECOMPONENT_H
#define CLIENTACTIVECOMPONENT_H

#include "vfmoduleparameter.h"
#include <QTimer>

/**
 * @brief ClientActiveComponent is a simple helper fo performance/energy
 * management of modules that create status data a view wants to see
 * realtime-alike (but that costs..). Once the view is no more visible,
 * module can go down to slow mode. To achieve dynamic switching, module needs
 * to be informed about demanding clients.
 * And it works like this:
 * * A module creates an extra integer component
 * * This component is passed to init() together with an incative timeout
 *   duration
 * * A visible view (or any other client wanting full-speed) increments
 *   the component peroidically so that ClientActiveComponent does not
 *   run into timeout
 * * Module is notified by signal clientActiveStateChanged or can query state
 *   by areClientsActive().
 */
class ClientActiveComponent : public QObject
{
    Q_OBJECT
public:
    explicit ClientActiveComponent(QObject *parent = nullptr);
    /**
     * @brief init: Init client notification component
     * @param component: The component to watch for client changes
     * @param iInactiveTimeoutMs: Maximum time inactive clients are detected as gone
     */
    void init(VfModuleParameter* component, int iInactiveTimeoutMs = 2000);
    /**
     * @brief areClientsActive: Client activity getter
     * @return guess...
     */
    bool areClientsActive() { return m_bClientActive; }
signals:
    /**
     * @brief clientActiveStateChanged: Once the client active state changes, this signal is fired
     * @param bActive
     */
    void clientActiveStateChanged(bool bActive);

private slots:
    void onClientNotification(QVariant);
    void onClientTimeout();

private:
    VfModuleParameter* m_pClientActiveNotifyPar = nullptr;
    bool m_bClientActive = false;
    QTimer m_inactiveTimer;
};

#endif // CLIENTACTIVECOMPONENT_H
