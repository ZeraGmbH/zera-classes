#ifndef RANGEMODULE_H
#define RANGEMODULE_H

#include "basemeasmodule.h"

class VfModuleMetaData;

namespace RANGEMODULE {

class cRangeMeasChannel;
class cRangeModuleConfiguration;
class cRangeModuleMeasProgram;
class cAdjustManagement;
class cRangeObsermatic;

class cRangeModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "RangeModule";
    static constexpr const char* BaseSCPIModuleName = "RNG";
    cRangeModule(MeasurementModuleFactoryParam moduleParam);
    QByteArray getConfiguration() const override;
    virtual cRangeMeasChannel* getMeasChannel(QString name); // also used for callback

protected:
    cRangeModuleMeasProgram *m_pMeasProgram = nullptr; // our measuring program, lets say the working horse
    cAdjustManagement * m_pAdjustment = nullptr; // our justifying and normation program
    cRangeObsermatic *m_pRangeObsermatic = nullptr; // our range handling
    QList<cRangeMeasChannel*> m_rangeMeasChannelList; // our meas channels
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

protected slots:
    void activationStart() override;
    void activationExec() override;
    void activationDone() override;
    void activationFinished() override;

    void deactivationStart() override;
    void deactivationExec() override;
    void deactivationDone() override;
    void deactivationFinished() override;

private:
    VfModuleMetaData* m_pChannelCountInfo;
    VfModuleMetaData* m_pGroupCountInfo;
    qint32 m_nActivationIt;

private slots:
    void setPeakRmsAndFrequencyValues(const QVector<float>* const values);
};

}

#endif // RANGEMODULE_H
