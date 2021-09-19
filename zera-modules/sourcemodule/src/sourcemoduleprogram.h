#ifndef SOURCEMODULEPROGRAM_H
#define SOURCEMODULEPROGRAM_H

#include "basemeasworkprogram.h"
#include "sourcemoduleconfiguration.h"

class cVeinModuleParameter;
class cVeinModuleActvalue;

namespace SOURCEMODULE
{

class cSourceModule;

class cSourceModuleProgram: public cBaseMeasWorkProgram
{
    Q_OBJECT

public:
    cSourceModuleProgram(cSourceModule* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
    virtual void generateInterface(); // here we export our interface (entities)

public slots: // Make cBaseMeasWorkProgram happy...
    virtual void start() {}
    virtual void stop() {}

private:
    configuration* getConfigXMLWrapper();

    cSourceModule* m_pModule; // the module we live in
    cVeinModuleActvalue* m_pVeinMaxCountAct;
    cVeinModuleParameter* m_pVeinDemoSourceCount;

private slots:
    // vein change handlers
    void newDemoSourceCount(QVariant maxCount);

};
}

#endif // SOURCEMODULEPROGRAM_H
