#ifndef ELECTRONICBURDENMODULE_H
#define ELECTRONICBURDENMODULE_H

#include "electronicburdenmoduleconfiguration.h"
#include <basemeasmodule.h>

namespace ELECTRONICBURDENMODULE
{
class ElectronicBurdenModuleConfiguration;
class ElectronicBurdenModuleProgram;

class ElectronicBurdenModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "ElBurdenModule";
    static constexpr const char* BaseSCPIModuleName = "EBU";

    ElectronicBurdenModule(quint8 modnr, int entityId, VeinEvent::StorageSystem *storagesystem, QObject* parent = nullptr);
    virtual QByteArray getConfiguration() const;

protected:
    virtual void doConfiguration(QByteArray xmlConfigData); // here we have to do our configuration
    virtual void setupModule(); // after xml configuration we can setup and export our module
    virtual void startMeas(); // we make the measuring program start here
    virtual void stopMeas();
    ElectronicBurdenModuleProgram *m_pProgram; // our program, lets say the working horse
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
