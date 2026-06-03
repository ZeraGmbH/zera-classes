#ifndef ADJUSTMENTMODULECONFIGURATION_H
#define ADJUSTMENTMODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

class cAdjustmentModuleConfigData;

class cAdjustmentModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    cAdjustmentModuleConfiguration();
    ~cAdjustmentModuleConfiguration();
    virtual void setConfiguration(const QByteArray& xmlString) override;
    virtual QByteArray exportConfiguration() override; // exports conf. and parameters to xml
    cAdjustmentModuleConfigData* getConfigurationData();
protected slots:
    virtual void configXMLInfo(const QString &key) override;
private slots:
    void completeConfiguration(bool ok);
private:
    cAdjustmentModuleConfigData *m_pAdjustmentModulConfigData = nullptr;  // configuration
};

#endif // ADJUSTMENTMODULCONFIGURATION_H
