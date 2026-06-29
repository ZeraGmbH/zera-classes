#ifndef POWER1MODULECONFIGURATION_H
#define POWER1MODULECONFIGURATION_H

#include "basemoduleconfiguration.h"
#include "power1moduleconfigdata.h"

namespace POWER1MODULE
{

class cPower1ModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit cPower1ModuleConfiguration(const QByteArray& xmlString);
    QByteArray exportConfiguration() const override;
    cPower1ModuleConfigData* getConfigData();

private slots:
    void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);
private:
    void setConfiguration(const QByteArray& xmlString);
    void addMeasSys(const QString &val);
    cPower1ModuleConfigData m_configData;
};

}

#endif // POWER1MODULCONFIGURATION_H
