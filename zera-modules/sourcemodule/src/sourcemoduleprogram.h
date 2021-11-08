#ifndef SOURCEMODULEPROGRAM_H
#define SOURCEMODULEPROGRAM_H

#include <vf-cpp-rpc.h>
#include "basemeasworkprogram.h"
#include "sourcemoduleconfiguration.h"
#include "sourceveininterface.h"
class cVeinModuleActvalue;
class cVeinModuleParameter;
class cJsonParamValidator;

namespace SOURCEMODULE
{

class cSourceModule;
class cSourceDevice;
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
    QVariant RPC_ScanInterface(QVariantMap p_params);

private:
    configuration* getConfigXMLWrapper();

    cSourceDeviceManager* m_pSourceDeviceManager;

    cSourceModule* m_pModule; // the module we live in

    cVeinModuleActvalue* m_pVeinMaxCountAct;
    cVeinModuleActvalue* m_pVeinCountAct;

    cVeinModuleParameter* m_pVeinDemoSourceCount;

    QVector<cSourceVeinInterface*> m_arrVeinSourceInterfaces;

    VfCpp::cVeinModuleRpc::Ptr m_sharedPtrRpcScanInterface;

private slots:
    void onSourceScanFinished(int slotPosition, cSourceDevice *device, QUuid uuid, QString errMsg);
    void onSourceDeviceRemoved(int slotPosition);

    // vein change handlers
    void newDemoSourceCount(QVariant demoCount);
};
}

#endif // SOURCEMODULEPROGRAM_H
