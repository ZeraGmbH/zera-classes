#ifndef RANGEMODULE_H
#define RANGEMODULE_H

#include "rangemodulemeasprogram.h"
#include "rangemeaschannel.h"
#include "rangeobsermatic.h"
#include "basemeasmodule.h"

namespace RANGEMODULE {

class cAdjustManagement;

class cRangeModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "RangeModule";
    static constexpr const char* BaseSCPIModuleName = "RNG";

    explicit cRangeModule(ModuleFactoryParam moduleParam);
    cRangeMeasChannel* getMeasChannel(const QString &name); // also used for callback

private slots:
    void activationFinished() override;
    void deactivationStart() override;
    void setPeakRmsAndFrequencyValues(const QVector<float>* const values);
private:
    void setupModule() override; // after xml configuration we can setup and export our module
    TaskTemplatePtr getModuleSetUpTask() override;
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    cRangeModuleMeasProgram *m_pMeasProgram = nullptr;
    cAdjustManagement *m_pAdjustment = nullptr; // our justifying and normation program
    cRangeObsermatic *m_pRangeObsermatic = nullptr; // our range handling
    QList<cRangeMeasChannel*> m_rangeMeasChannelList; // our meas channels
    VfModuleMetaData* m_pChannelCountInfo = nullptr;
    VfModuleMetaData* m_pGroupCountInfo = nullptr;
};

}

#endif // RANGEMODULE_H
