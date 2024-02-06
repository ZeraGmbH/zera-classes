#ifndef MODULEOBJECT_H
#define MODULEOBJECT_H
#include <QObject>
#include <QList>
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

    virtual void setConfiguration(QByteArray xmlConfigData) = 0; // here we set configuration and parameters
    virtual QByteArray getConfiguration() const = 0;
    virtual bool isConfigured() const = 0;

    virtual void startModule() = 0;
    virtual void stopModule() = 0;

    QStateMachine m_ActivationMachine; // we use statemachine for module activation
    QStateMachine m_DeactivationMachine; // and deactivation
signals:
    // signals to be used by activation and deactivation statemachine when ready
    void activationReady();
    void deactivationReady();

    // signals for modulemanager notification
    void moduleActivated();
    void moduleDeactivated();
    void addEventSystem(VeinEvent::EventSystem*);
    void parameterChanged();

protected:
    QStateMachine m_stateMachine;

    QState *m_pStateIdle;
    QState *m_pStateConfigure;
    QState *m_pStateRun;
    QState *m_pStateStop;
    QFinalState *m_pStateFinished;

    QString m_sModuleName;
    QString m_sSCPIModuleName;
private:
    const int m_nEntityId;
    const quint16 m_moduleNo;
};
}

#endif // MODULEOBJECT_H
