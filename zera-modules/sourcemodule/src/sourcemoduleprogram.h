#ifndef SOURCEMODULEPROGRAM_H
#define SOURCEMODULEPROGRAM_H

#include <QUuid>
#include <vf-cpp-rpc.h>
#include "basemeasworkprogram.h"
#include "sourcemoduleconfiguration.h"
#include "sourceveininterface.h"

class cVeinModuleActvalue;
class cVeinModuleParameter;
class cJsonParamValidator;
class SourceModule;
class SourceDeviceVein;
class SourceDeviceManager;

class SourceModuleProgram: public cBaseMeasWorkProgram
{
    Q_OBJECT
public:
    SourceModuleProgram(SourceModule* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
    virtual ~SourceModuleProgram();
    virtual void generateInterface() override; // here we export our interface (entities)

public slots: // Make cBaseMeasWorkProgram happy...
    virtual void start() override {}
    virtual void stop() override {}
    QVariant RPC_ScanInterface(QVariantMap p_params);
    QVariant RPC_CloseSource(QVariantMap p_params);

private:
    configuration* getConfigXMLWrapper();
    void updateDemoCount();

    SourceDeviceManager* m_pSourceDeviceManager = nullptr;

    SourceModule* m_pModule; // the module we live in

    cVeinModuleActvalue* m_pVeinMaxCountAct;
    cVeinModuleActvalue* m_pVeinCountAct;

    cVeinModuleParameter* m_pVeinDemoSourceCount;

    QVector<SourceVeinInterface*> m_arrVeinSourceInterfaces;

    VfCpp::cVeinModuleRpc::Ptr m_sharedPtrRpcScanInterface;
    VfCpp::cVeinModuleRpc::Ptr m_sharedPtrRpcRemoveInterface;

    bool m_bDeafenDemoChange = false;

private slots:
    void onSourceScanFinished(int slotPosition, SourceDeviceVein *device, QUuid uuid, QString errMsg);
    void onSourceDeviceRemoved(int slot, QUuid uuid, QString errMsg);

    // vein change handlers
    void newDemoSourceCount(QVariant getDemoCount);
};

#endif // SOURCEMODULEPROGRAM_H
