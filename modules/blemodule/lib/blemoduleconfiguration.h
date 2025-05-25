#ifndef BLEMODULECONFIGURATION_H
#define BLEMODULECONFIGURATION_H

#include "blemoduleconfigdata.h"
#include "basemoduleconfiguration.h"

namespace BLEMODULE
{

enum moduleconfigstate
{
    setBluetoothOn,
    setMacAddress
};

// moduleconfiguration holds configuration data as well as parameter

class cBleModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    cBleModuleConfiguration();
    ~cBleModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cBleModuleConfigData* getConfigurationData();

protected slots:
    virtual void configXMLInfo(QString key);
    virtual void completeConfiguration(bool ok);

private:
    cBleModuleConfigData *m_pBleModulConfigData = nullptr;  // configuration
};

}

#endif // BLEMODULECONFIGURATION_H
