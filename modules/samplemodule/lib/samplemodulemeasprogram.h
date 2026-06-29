#ifndef SAMPLEMODULEMEASPROGRAM_H
#define SAMPLEMODULEMEASPROGRAM_H

#include "pcbserviceconnection.h"
#include "pllautomatic.h"
#include "samplemoduleconfigdata.h"
#include "moduleactivist.h"
#include "vfmoduleparameter.h"
#include <taskcontainerqueue.h>

namespace SAMPLEMODULE
{

class cSampleModule;

class cSampleModuleMeasProgram : public cModuleActivist
{
    Q_OBJECT
public:
    explicit cSampleModuleMeasProgram(cSampleModule* module);

    void generateVeinInterface() override;
    void activate() override;
    void deactivate() override;

private slots:
    void onVeinPllChannelChanged(const QVariant &channelAlias);
    void onVeinPllAutoChanged(const QVariant &pllauto);
    void onPllChannelChanged(const QString &channelMName);
private:
    QString getAlias(const QString &channelMName);
    void setPllChannelValidator();
    void trySendPllChannel(const QString &channelMName);
    void startSetPllChannel(const QString &channelMName);
    void setVeinPllChannelPesistent(const QString &channelMName);

    cSampleModule* m_module = nullptr;
    cObsermaticConfPar &m_obsermaticConfig;
    PcbServiceConnection m_pcbConnection;

    // we need some entities for our interface
    VfModuleParameter* m_pVeinPllChannelAlias = nullptr;
    VfModuleParameter* m_pParPllAutomaticOnOff = nullptr;
    VfModuleComponent* m_pPllFixed = nullptr;

    PllAutomatic m_pllAutomatic;
    TaskContainerQueue m_pendingTasks;
};

}

#endif // SAMPLEMODULEMEASPROGRAM_H
