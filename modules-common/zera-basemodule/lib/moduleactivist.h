#ifndef MODULEACTIVIST_H
#define MODULEACTIVIST_H

#include <QObject>
#include <QStateMachine>
#include <QHash>

// pure virtual class for all objects living in a module, which generate an interface
// and/or which can do something after it got activated
class cModuleActivist: public QObject
{
    Q_OBJECT
public:
    cModuleActivist(QString notifyHeaderString);
    virtual ~cModuleActivist(){}
signals:
    void activated(); // is emitted after the activist is completely activated
    void activationContinue(); // for activist progress in activation statemachine
    void activationLoop();
    void deactivated();
    void deactivationContinue();
    void deactivationLoop();
public slots:
    virtual void activate(); // here we query our properties and activate ourself
    virtual void deactivate(); // what do you think ? yes you're right
    virtual void generateInterface() = 0; // here we export our interface (entities)
protected:
    void notifyError(QVariant value);

    bool m_bActive = false;
    QString m_notifyHeaderString;
    QStateMachine m_activationMachine;
    QStateMachine m_deactivationMachine;
};

#endif // MODULEACITIVIST_H
