#ifndef ELECTRONICBURDENMODULEPROGRAM_H
#define ELECTRONICBURDENMODULEPROGRAM_H

#include <QUuid>
#include <vf-cpp-rpc.h>
#include "basemeasworkprogram.h"
#include "electronicburdenmoduleconfiguration.h"
#include "veininterface.h"

class cVeinModuleActvalue;
class cVeinModuleParameter;
class cJsonParamValidator;

namespace ELECTRONICBURDENMODULE
{

class ElectronicBurdenModule;

class ElectronicBurdenModuleProgram: public cBaseMeasWorkProgram
{
    Q_OBJECT
public:
    ElectronicBurdenModuleProgram(ElectronicBurdenModule* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
    virtual ~ElectronicBurdenModuleProgram();
    virtual void generateInterface() override; // here we export our interface (entities)

public slots: // Make cBaseMeasWorkProgram happy...
    virtual void start() override {}
    virtual void stop() override {}
    void newDemoTest(QVariant val);

private:
    configuration* getConfigXMLWrapper();

    ElectronicBurdenModule* m_pModule; // the module we live in
    cVeinModuleParameter* m_pVeinDemoTest = nullptr;
    VeinInterface* m_pVeinInterface = nullptr;

private slots:

};
}

#endif // ELECTRONICBURDENMODULEPROGRAM_H
