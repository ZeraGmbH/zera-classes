#ifndef POWER3MODULECONFIGURATION_H
#define POWER3MODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

namespace POWER3MODULE
{

enum moduleconfigstate
{
    setInputModule,
    setSystemCount,

    setMeasSystem1 = 16, // we leave some place for additional cmds

    setnext = setMeasSystem1 + 16
};



class cPower3ModuleConfigData;

// moduleconfiguration holds configuration data as well as parameter

class cPower3ModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    cPower3ModuleConfiguration();
    ~cPower3ModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cPower3ModuleConfigData* getConfigurationData();

protected slots:
    virtual void configXMLInfo(QString key);
    virtual void completeConfiguration(bool ok);

private:
    cPower3ModuleConfigData *m_pPower3ModulConfigData = nullptr;  // configuration
};

}

#endif // POWER3MODULCONFIGURATION_H
