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
    void setConfiguration(const QByteArray& xmlString) override;
    QByteArray exportConfiguration() override;
    cBleModuleConfigData* getConfigurationData();

private slots:
    void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);

private:
    cBleModuleConfigData *m_pBleModulConfigData = nullptr;
};

}

#endif // BLEMODULECONFIGURATION_H
