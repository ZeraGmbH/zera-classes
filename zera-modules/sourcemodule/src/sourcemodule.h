#ifndef SOURCEMODULE_H
#define SOURCEMODULE_H

#include <QObject>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QList>

#include "basemeasmodule.h"
#include "sourcemoduleconfiguration.h"

#define BaseModuleName "SourceModule"
#define BaseSCPIModuleName "SRC"

class SourceModuleConfiguration;
class SourceModuleProgram;

class SourceModule : public cBaseMeasModule
{
Q_OBJECT

public:
    SourceModule(quint8 modnr, Zera::Proxy::cProxy* proxi, int entityId, VeinEvent::StorageSystem *storagesystem, QObject* parent = 0);
    virtual QByteArray getConfiguration() const;

protected:
    SourceModuleProgram *m_pProgram; // our program, lets say the working horse

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

#endif // SOURCEMODULE_H
