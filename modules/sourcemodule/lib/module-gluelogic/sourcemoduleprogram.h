#ifndef SOURCEMODULEPROGRAM_H
#define SOURCEMODULEPROGRAM_H

#include "basemeasworkprogram.h"
#include "sourceveininterface.h"
#include "sourcedevicemanager.h"
#include <vf-cpp-rpc.h>
#include <QUuid>

class SourceModule;

class SourceModuleProgram: public cBaseMeasWorkProgram
{
    Q_OBJECT
public:
    SourceModuleProgram(SourceModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);
    virtual ~SourceModuleProgram();
    void generateVeinInterface() override;
    void startDestroy();
signals:
    void sigLastSourceRemoved();
public slots:
    // Make cBaseMeasWorkProgram happy...
    void start() override {}
    void stop() override {}

    QVariant RPC_ScanInterface(QVariantMap p_params);
    QVariant RPC_CloseSource(QVariantMap p_params);
private:
    void updateDemoCount();
    SourceDeviceManager* m_pSourceDeviceManager = nullptr;
    SourceModule* m_pModule; // the module we live in

    VfModuleComponent* m_pVeinMaxCountAct;
    VfModuleComponent* m_pVeinCountAct;
    VfModuleParameter* m_pVeinDemoSourceCount = nullptr;
    VfCpp::cVeinModuleRpc::Ptr m_sharedPtrRpcScanInterface;
    VfCpp::cVeinModuleRpc::Ptr m_sharedPtrRpcRemoveInterface;

    QVector<SourceVeinInterface*> m_arrVeinIoInterfaces;
    bool m_bDeafenDemoChange = false;
private slots:
    void onSourceScanFinished(int slotPosition, QUuid uuid, QString errorMsg);
    void onSourceDeviceRemoved(int slot, QUuid uuid, QString errorMsg);
    void newDemoSourceCount(QVariant getDemoCount);
};

#endif // SOURCEMODULEPROGRAM_H
