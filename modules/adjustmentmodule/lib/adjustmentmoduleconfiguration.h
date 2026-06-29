#ifndef ADJUSTMENTMODULECONFIGURATION_H
#define ADJUSTMENTMODULECONFIGURATION_H

#include "basemoduleconfiguration.h"
#include "adjustmentmoduleconfigdata.h"

class cAdjustmentModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit cAdjustmentModuleConfiguration(const QByteArray& xmlString);

    QByteArray exportConfiguration() const override;
    cAdjustmentModuleConfigData* getConfigData();

private slots:
    void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);
private:
    void setConfiguration(const QByteArray& xmlString);
    cAdjustmentModuleConfigData m_configData;
};

#endif // ADJUSTMENTMODULCONFIGURATION_H
