#ifndef MODULEACTIVIST_H
#define MODULEACTIVIST_H

#include "vfmoduleerrorcomponent.h"
#include "notificationfactory.h"
#include <QObject>
#include <QJsonArray>
#include <QStateMachine>
#include <QHash>
#include <functional>

// pure virtual class for all objects living in a module, which generate an interface
// and/or which can do something after it got activated
class cModuleActivist: public QObject
{
    Q_OBJECT
public:
    cModuleActivist(){m_bActive = false;}
    virtual ~cModuleActivist(){}
signals:
    void activated(); // is emitted after the activist is completely activated
    void activationContinue(); // for activist progress in activation statemachine
    void activationLoop();
    void activationRepeat();
    void activationSkip();
    void activationError();
    void deactivated();
    void deactivationContinue();
    void deactivationLoop();
    void deactivationSkip();
    void deactivationError();
    void dataAquisitionContinue();
    void setupContinue();
    void interruptContinue();
    void executionError();
    void errMsg(QVariant value, int dest = globalDest);
public slots:
    virtual void activate(); // here we query our properties and activate ourself
    virtual void deactivate(); // what do you think ? yes you're right
    virtual void generateInterface() = 0; // here we export our interface (entities)
    void createNotification(QVariant msg);
    void removeNotification(int id);
protected:
    void notifyActivationError(QVariant value, int dest = globalDest);
    void notifyDeactivationError(QVariant value, int dest = globalDest);
    void notifyExecutionError(QVariant value, int dest = globalDest);
    bool handleFinishCallback(int cmdNumber, quint8 reply, QVariant answer);

    bool m_bActive;
    QStateMachine m_activationMachine;
    QStateMachine m_deactivationMachine;
    QHash<int, std::function<void(quint8 reply, QVariant answer)>> m_cmdFinishCallbacks;
    NotificationFactory *m_notificationFactory;
};

#endif // MODULEACITIVIST_H
