#ifndef ELECTRONICBURDENMODULE_H
#define ELECTRONICBURDENMODULE_H

#include <QObject>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QList>

#include "basemeasmodule.h"
#include "electronicburdenmoduleconfiguration.h"


namespace ELECTRONICBURDENMODULE
{


#define BaseModuleName "ElBurdenModule"
#define BaseSCPIModuleName "EBU"

class ElectronicBurdenModuleConfiguration;
class ElectronicBurdenModuleProgram;

class ElectronicBurdenModule : public cBaseMeasModule
{
Q_OBJECT

public:
    ElectronicBurdenModule(quint8 modnr, Zera::Proxy::cProxy* proxi, int entityId, VeinEvent::StorageSystem *storagesystem, QObject* parent = 0);
    virtual QByteArray getConfiguration() const;

protected:
    ElectronicBurdenModuleProgram *m_pProgram; // our program, lets say the working horse

    virtual void doConfiguration(QByteArray xmlConfigData); // here we have to do our configuration
    virtual void setupModule(); // after xml configuration we can setup and export our module
    virtual void startMeas(); // we make the measuring program start here
    virtual void stopMeas();

private slots:
    // entered immediately - no interim states for now
    void activationFinished();
    void deactivationFinished();

    // sigh - we have to add overrides to not being pure virtual
    virtual void activationStart() {}
    virtual void activationExec() {}
    virtual void activationDone() {}

    virtual void deactivationStart() {}
    virtual void deactivationExec() {}
    virtual void deactivationDone() {}
};

}

#endif // ELECTRONICBURDENMODULE_H
