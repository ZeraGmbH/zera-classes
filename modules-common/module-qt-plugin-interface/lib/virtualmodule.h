#ifndef MODULEOBJECT_H
#define MODULEOBJECT_H

#include <QStateMachine>
#include <QFinalState>

namespace VeinEvent {
    class EventSystem;
}

namespace ZeraModules
{
class VirtualModule : public QObject
{
    Q_OBJECT
public:
    explicit VirtualModule(quint16 moduleNo, int entityId) :
        m_nEntityId(entityId),
        m_moduleNo(moduleNo)
    {}
    virtual ~VirtualModule() = default;
    int getEntityId() { return m_nEntityId; }
    quint16 getModuleNr() { return m_moduleNo; }
    QString getVeinModuleName() { return m_sModuleName; };
    QString getSCPIModuleName() { return m_sSCPIModuleName; };

    virtual QByteArray getConfiguration() const = 0;

    virtual void startModule() = 0;
    virtual void stopModule() = 0;
    virtual void startDestroy() = 0;

signals:
    // signals for modulemanager notification
    void moduleActivated();
    void moduleDeactivated();
    void addEventSystem(VeinEvent::EventSystem*);
    void addEventSubSystem(VeinEvent::EventSystem*);
    void removeEventSubSystem(VeinEvent::EventSystem*);
    void parameterChanged();

protected:
    QString m_sModuleName;
    QString m_sSCPIModuleName;
private:
    const int m_nEntityId;
    const quint16 m_moduleNo;
};
}

#endif // MODULEOBJECT_H
