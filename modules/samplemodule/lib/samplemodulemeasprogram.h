#ifndef SAMPLEMODULEMEASPROGRAM_H
#define SAMPLEMODULEMEASPROGRAM_H

#include "basemoduleconfiguration.h"
#include "pcbserviceconnection.h"
#include "samplemoduleconfigdata.h"
#include "moduleactivist.h"
#include "vfmoduleparameter.h"
#include <taskcontainersequence.h>

namespace SAMPLEMODULE
{

class cSampleModule;

class cSampleModuleMeasProgram : public cModuleActivist
{
    Q_OBJECT
public:
    cSampleModuleMeasProgram(cSampleModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);

    void generateVeinInterface() override;
    void activate() override;
    void deactivate() override;
private slots:
    void onVeinPllChannelChanged(QVariant channelAlias);
    void onVeinPllAutoChanged(QVariant pllauto);
private:
    cSampleModuleConfigData* getConfData();
    QString getAlias(const QString &channelMName);
    void setPllChannelValidator();
    void pllAutomatic();
    void trySendPllChannel(const QString &channelAlias);
    void startSetPllChannel(const QString &channelMName);
    void setVeinPllChannelPesistent(const QString &channelMName);

    cSampleModule* m_module;
    std::shared_ptr<BaseModuleConfiguration> m_pConfiguration;
    cObsermaticConfPar &m_obsermaticConfig;
    PcbServiceConnection m_pcbConnection;

    // we need some entities for our interface
    VfModuleParameter* m_pVeinPllChannelAlias = nullptr;
    VfModuleParameter* m_pParPllAutomaticOnOff = nullptr;
    VfModuleComponent* m_pPllFixed = nullptr;
    VfModuleComponent* m_pPllSignal = nullptr; // we make the signal public for easy connection within module

    TaskContainerSequence m_pendingTasks;
};

}

#endif // SAMPLEMODULEMEASPROGRAM_H
