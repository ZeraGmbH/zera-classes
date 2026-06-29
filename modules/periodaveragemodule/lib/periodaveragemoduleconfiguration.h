#ifndef PERIODAVERAGEMODULECONFIGURATION_H
#define PERIODAVERAGEMODULECONFIGURATION_H

#include "periodaveragemoduleconfigdata.h"
#include "basemoduleconfiguration.h"
#include <QByteArray>

namespace PERIODAVERAGEMODULE
{

class PeriodAverageModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit PeriodAverageModuleConfiguration(const QByteArray &xmlString);

    QByteArray exportConfiguration() const override;
    PeriodAverageModuleConfigData* getConfigData();

private slots:
    void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);
private:
    void setConfiguration(const QByteArray &xmlString);
    PeriodAverageModuleConfigData m_configData;
};

}

#endif // PERIODAVERAGEMODULECONFIGURATION_H
