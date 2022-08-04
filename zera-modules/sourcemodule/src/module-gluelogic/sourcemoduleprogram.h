#ifndef SOURCEMODULEPROGRAM_H
#define SOURCEMODULEPROGRAM_H

#include "basemeasworkprogram.h"
#include "sourcemoduleconfiguration.h"
#include "vein/sourceveininterface.h"
#include "source-device/sourcedevicemanager.h"
#include <vf-cpp-rpc.h>
#include <QUuid>

class SourceModule;

class SourceModuleProgram: public cBaseMeasWorkProgram
{
    Q_OBJECT
public:
    SourceModuleProgram(SourceModule* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
    virtual ~SourceModuleProgram();
    virtual void generateInterface() override; // here we export our interface (entities)
    void startDestroy();
signals:
    void sigLastSourceRemoved();
public slots:
    // Make cBaseMeasWorkProgram happy...
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
    VfCpp::cVeinModuleRpc::Ptr m_sharedPtrRpcScanInterface;
    VfCpp::cVeinModuleRpc::Ptr m_sharedPtrRpcRemoveInterface;

    QVector<SourceVeinInterface*> m_arrVeinIoInterfaces;
    bool m_bDeafenDemoChange = false;
private slots:
    void onSourceScanFinished(int slotPosition, QUuid uuid, QString errMsg);
    void onSourceDeviceRemoved(int slot, QUuid uuid, QString errMsg);
    void newDemoSourceCount(QVariant getDemoCount);
};

#endif // SOURCEMODULEPROGRAM_H
