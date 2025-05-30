#ifndef ADJUSTMENTMODULECONFIGURATION_H
#define ADJUSTMENTMODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

enum moduleconfigstate
{
    setAngleReferenceEntity,
    setAngleReferenceComponent,
    setFrequencyReferenceEntity,
    setFrequencyReferenceComponent,
    setChannelCount,

    setChn1Chn = 16, // we leave some place for additional base cmds and commands for up 32 channels

    setChn1AmplInfoAvail = setChn1Chn +32,
    setChn1AmplInfoEntity = setChn1AmplInfoAvail +32,
    setChn1AmplInfoComponent = setChn1AmplInfoEntity + 32,

    setChn1PhaseInfoAvail = setChn1AmplInfoComponent + 32,
    setChn1PhaseInfoEntity = setChn1PhaseInfoAvail + 32,
    setChn1PhaseInfoComponent = setChn1PhaseInfoEntity + 32,

    setChn1OffsInfoAvail = setChn1PhaseInfoComponent + 32,
    setChn1OffsInfoEntity = setChn1OffsInfoAvail + 32,
    setChn1OffsInfoComponent = setChn1OffsInfoEntity + 32,

    setChn1RangeInfoAvail = setChn1OffsInfoComponent + 32,
    setChn1RangeInfoEntity = setChn1RangeInfoAvail + 32,
    setChn1RangeInfoComponent = setChn1RangeInfoEntity + 32,

    setnext = setChn1RangeInfoComponent + 32
};

class cAdjustmentModuleConfigData;

// moduleconfiguration holds configuration data as well as parameter
class cAdjustmentModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    cAdjustmentModuleConfiguration();
    ~cAdjustmentModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString) override;
    virtual QByteArray exportConfiguration() override; // exports conf. and parameters to xml
    cAdjustmentModuleConfigData* getConfigurationData();
protected slots:
    virtual void configXMLInfo(QString key) override;
private slots:
    void completeConfiguration(bool ok);
private:
    cAdjustmentModuleConfigData *m_pAdjustmentModulConfigData = nullptr;  // configuration
};

#endif // ADJUSTMENTMODULCONFIGURATION_H
