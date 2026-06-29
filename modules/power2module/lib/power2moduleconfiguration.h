#ifndef POWER2MODULECONFIGURATION_H
#define POWER2MODULECONFIGURATION_H

#include "basemoduleconfiguration.h"
#include "power2moduleconfigdata.h"

namespace POWER2MODULE
{

class cPower2ModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit cPower2ModuleConfiguration(const QByteArray& xmlString);

    QByteArray exportConfiguration() const override;
    cPower2ModuleConfigData* getConfigData();

private slots:
    void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);
private:
    void setConfiguration(const QByteArray& xmlString);
    void addMeasSys(const QString &val);
    cPower2ModuleConfigData m_configData;
};

}

#endif // POWER2MODULCONFIGURATION_H
