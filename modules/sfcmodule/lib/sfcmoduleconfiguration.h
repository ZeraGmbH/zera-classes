#ifndef SFCMODULECONFIGURATION_H
#define SFCMODULECONFIGURATION_H

#include "sfcmoduleconfigdata.h"
#include "basemoduleconfiguration.h"

namespace SFCMODULE
{

enum moduleconfigstate
{
    setDutInputCount,

    setDutInputPar,

    setDutInput1Name = 32,
};

// moduleconfiguration holds configuration data as well as parameter

class cSfcModuleConfiguration : public BaseModuleConfiguration
{
    Q_OBJECT
public:
    cSfcModuleConfiguration();
    ~cSfcModuleConfiguration();
    virtual void setConfiguration(const QByteArray& xmlString) override;
    virtual QByteArray exportConfiguration() override; // exports conf. and parameters to xml
    cSfcModuleConfigData *getConfigurationData();

protected slots:
    virtual void configXMLInfo(const QString &key) override;
    virtual void completeConfiguration(bool ok);

private:
    cSfcModuleConfigData *m_pSfcModulConfigData = nullptr; // configuration
};

}

#endif // SFCMODULECONFIGURATION_H
