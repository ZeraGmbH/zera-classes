#ifndef PERIODAVERAGEMODULECONFIGURATION_H
#define PERIODAVERAGEMODULECONFIGURATION_H

#include "periodaveragemoduleconfigdata.h"
#include "basemoduleconfiguration.h"
#include <QByteArray>

namespace PERIODAVERAGEMODULE
{

// rangemoduleconfiguration holds configuration data as well as parameter

class PeriodAverageModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    PeriodAverageModuleConfiguration();
    ~PeriodAverageModuleConfiguration();
    void setConfiguration(const QByteArray &xmlString) override;
    virtual QByteArray exportConfiguration() override; // exports conf. and parameters to xml
    PeriodAverageModuleConfigData* getConfigurationData();

private slots:
    void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);
private:
    PeriodAverageModuleConfigData *m_periodAverageModulConfigData = nullptr;
};

}

#endif // PERIODAVERAGEMODULECONFIGURATION_H
