#ifndef HOTPLUGCONTROLSMODULECONFIGURATION_H
#define HOTPLUGCONTROLSMODULECONFIGURATION_H

#include "basemoduleconfiguration.h"

namespace HOTPLUGCONTROLSMODULE
{

enum moduleconfigstate
{
};

class cHotplugControlsModuleConfigData;

class cHotplugControlsModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    cHotplugControlsModuleConfiguration();
    ~cHotplugControlsModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cHotplugControlsModuleConfigData* getConfigurationData();
protected slots:
    virtual void configXMLInfo(QString key);
    virtual void completeConfiguration(bool ok);
private:
    cHotplugControlsModuleConfigData *m_pHotplugControlsModuleConfigData = nullptr;  // configuration
};

}

#endif // HOTPLUGCONTROLSMODULECONFIGURATION_H
