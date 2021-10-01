#ifndef SOURCEMODULEPROGRAM_H
#define SOURCEMODULEPROGRAM_H

#include "basemeasworkprogram.h"
#include "sourcemoduleconfiguration.h"


class cVeinModuleActvalue;
class cVeinModuleParameter;
class cJsonParamValidator;

namespace SOURCEMODULE
{

class cSourceModule;
class cSourceDeviceManager;

class cSourceModuleProgram: public cBaseMeasWorkProgram
{
    Q_OBJECT

public:
    cSourceModuleProgram(cSourceModule* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
    virtual ~cSourceModuleProgram();
    virtual void generateInterface(); // here we export our interface (entities)

public slots: // Make cBaseMeasWorkProgram happy...
    virtual void start() {}
    virtual void stop() {}

private:
    configuration* getConfigXMLWrapper();

    cSourceDeviceManager* m_pSourceDeviceManager;

    cSourceModule* m_pModule; // the module we live in
    cVeinModuleActvalue* m_pVeinMaxCountAct;
    cVeinModuleActvalue* m_pVeinCountAct;
    cVeinModuleParameter* m_pVeinDemoSourceCount;

    QVector<cVeinModuleActvalue*> m_arrVeinSourceDeviceInfo;
    QVector<cVeinModuleParameter*> m_arrVeinSourceDeviceParameter;
    QVector<cJsonParamValidator*> m_arrVeinSourceDeviceParameterValidators;

private slots:
    void onSourceDeviceAdded(int slotPosition);
    void onSourceDeviceRemoved(int slotPosition);

    // vein change handlers
    void newDemoSourceCount(QVariant demoCount);

};
}

#endif // SOURCEMODULEPROGRAM_H
