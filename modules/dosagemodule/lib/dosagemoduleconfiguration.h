#ifndef DOSAGEMODULECONFIGURATION_H
#define DOSAGEMODULECONFIGURATION_H

#include "dosagemoduleconfigdata.h"
#include "basemoduleconfiguration.h"

namespace DOSAGEMODULE
{

enum moduleconfigstate
{
};

// moduleconfiguration holds configuration data as well as parameter

class cDosageModuleConfiguration : public BaseModuleConfiguration
{
    Q_OBJECT
public:
    cDosageModuleConfiguration();
    ~cDosageModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cDosageModuleConfigData *getConfigurationData();

protected slots:
    virtual void configXMLInfo(QString key);
    virtual void completeConfiguration(bool ok);

private:
    cDosageModuleConfigData *m_pDosageModulConfigData; // configuration
};

}

#endif // DOSAGEMODULECONFIGURATION_H
