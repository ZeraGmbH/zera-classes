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
    explicit VirtualModule(QObject *qObjParent=0) : QObject(qObjParent) {}
    virtual ~VirtualModule() {}
    virtual void setConfiguration(QByteArray xmlConfigData)=0; // here we set configuration and parameters
    virtual QByteArray getConfiguration() const =0;
    virtual bool isConfigured() const =0;

    virtual void startModule()=0;
    virtual void stopModule()=0;

    QStateMachine m_ActivationMachine; // we use statemachine for module activation
    QStateMachine m_DeactivationMachine; // and deactivation

  signals:
    // signals to be used by activation and deactivation statemachine when ready
    void activationReady();
    void deactivationReady();

    // signals for modulemanager notification
    void moduleError(QString errorString);
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
  };
}

#endif // MODULEOBJECT_H
