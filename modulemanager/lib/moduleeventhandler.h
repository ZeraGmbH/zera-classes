#ifndef MODULEEVENTHANDLER_H
#define MODULEEVENTHANDLER_H

#include <ve_eventhandler.h>
#include <ve_eventsystem.h>
#include <QList>

class ModuleEventHandler : public VeinEvent::EventHandler
{
    Q_OBJECT
public:
    explicit ModuleEventHandler(QObject *parent = nullptr);
    /**
   * @brief sets the arbitration system, however the usage of the system depends on the define #ifdef DEVICE_ARBITRATION
   * @param t_arbitrationSystem
   */
    void setArbitrationSystem(VeinEvent::EventSystem *t_arbitrationSystem);
signals:

public slots:
    /**
   * @brief Used to add the modules to m_moduleSystems
   * @param t_eventSystem
   */
    void addSystem(VeinEvent::EventSystem *t_eventSystem);
    /**
   * @brief clears the m_moduleSystems
   */
    void clearSystems();

    //VeinEvent::EventHandler interface
protected:
    void customEvent(QEvent *t_event) override;

private:
    QList<VeinEvent::EventSystem *> m_moduleSystems;

    /**
   * @brief the arbitration system is the first to handle events
   * its processEvents is called before all m_moduleSystems and all VeinEvent::EventHandler::m_subsystems
   */
    VeinEvent::EventSystem *m_arbitration=nullptr;
};

#endif // MODULEEVENTHANDLER_H
