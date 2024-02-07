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
    virtual void doConfiguration(QByteArray xmlConfigData); // here we have to do our configuration
    virtual void setupModule(); // after xml configuration we can setup and export our module
    virtual void startMeas(); // we make the measuring program start here
    virtual void stopMeas();

protected slots:
    virtual void activationStart();
    virtual void activationExec();
    virtual void activationDone();
    virtual void activationFinished();

    virtual void deactivationStart();
    virtual void deactivationExec();
    virtual void deactivationDone();
    virtual void deactivationFinished();

private:
    VfModuleMetaData* m_pChannelCountInfo;
    VfModuleMetaData* m_pGroupCountInfo;
    qint32 m_nActivationIt;

private slots:
    void setPeakRmsAndFrequencyValues(const QVector<float>* const values);
};

}

#endif // RANGEMODULE_H
