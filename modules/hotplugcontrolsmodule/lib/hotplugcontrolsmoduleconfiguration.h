#ifndef HOTPLUGCONTROLSMODULECONFIGURATION_H
#define HOTPLUGCONTROLSMODULECONFIGURATION_H

#include "basemoduleconfiguration.h"
#include "hotplugcontrolsmoduleconfigdata.h"

namespace HOTPLUGCONTROLSMODULE
{

class cHotplugControlsModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit cHotplugControlsModuleConfiguration(const QByteArray& xmlString);

    virtual QByteArray exportConfiguration() const override;
    cHotplugControlsModuleConfigData* getConfigData();

private slots:
    void configXMLInfo(const QString &key) override;
    virtual void completeConfiguration(bool ok);
private:
    void setConfiguration(const QByteArray& xmlString);
    cHotplugControlsModuleConfigData m_configData;
};

}

#endif // HOTPLUGCONTROLSMODULECONFIGURATION_H
