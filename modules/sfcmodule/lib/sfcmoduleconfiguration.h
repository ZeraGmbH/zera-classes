#ifndef SFCMODULECONFIGURATION_H
#define SFCMODULECONFIGURATION_H

#include "sfcmoduleconfigdata.h"
#include "basemoduleconfiguration.h"

namespace SFCMODULE
{

enum moduleconfigstate
{
};

// moduleconfiguration holds configuration data as well as parameter

class cSfcModuleConfiguration : public BaseModuleConfiguration
{
    Q_OBJECT
public:
    cSfcModuleConfiguration();
    ~cSfcModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cSfcModuleConfigData *getConfigurationData();

protected slots:
    virtual void configXMLInfo(QString key);
    virtual void completeConfiguration(bool ok);

private:
    cSfcModuleConfigData *m_pSfcModulConfigData; // configuration
};

}

#endif // SFCMODULECONFIGURATION_H
