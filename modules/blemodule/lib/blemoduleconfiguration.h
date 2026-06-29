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

class cBleModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit cBleModuleConfiguration(const QByteArray &xmlString);

    QByteArray exportConfiguration() const override;
    cBleModuleConfigData* getConfigData();

private slots:
    void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);
private:
    void setConfiguration(const QByteArray& xmlString);
    cBleModuleConfigData m_configData;
};

}

#endif // BLEMODULECONFIGURATION_H
