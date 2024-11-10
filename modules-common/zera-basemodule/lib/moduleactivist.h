#ifndef MODULEACTIVIST_H
#define MODULEACTIVIST_H

#include <QObject>
#include <QStateMachine>
#include <QHash>
#include <functional>

// pure virtual class for all objects living in a module, which generate an interface
// and/or which can do something after it got activated
class cModuleActivist: public QObject
{
    Q_OBJECT
public:
    cModuleActivist(QString notifyInfo);
    virtual ~cModuleActivist(){}
signals:
    void activated(); // is emitted after the activist is completely activated
    void activationContinue(); // for activist progress in activation statemachine
    void activationLoop();
    void activationRepeat();
    void deactivated();
    void deactivationContinue();
    void deactivationLoop();
    void dataAquisitionContinue();
    void setupContinue();
    void interruptContinue();
public slots:
    virtual void activate(); // here we query our properties and activate ourself
    virtual void deactivate(); // what do you think ? yes you're right
    virtual void generateInterface() = 0; // here we export our interface (entities)
protected:
    void notifyError(QVariant value);
    bool handleFinishCallback(int cmdNumber, quint8 reply, QVariant answer);

    bool m_bActive = false;
    QString m_notifyInfo;
    QStateMachine m_activationMachine;
    QStateMachine m_deactivationMachine;
    QHash<int, std::function<void(quint8 reply, QVariant answer)>> m_cmdFinishCallbacks;
};

#endif // MODULEACITIVIST_H
