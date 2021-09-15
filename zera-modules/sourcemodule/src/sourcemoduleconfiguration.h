#ifndef SOURCEMODULECONFIGURATION_H
#define SOURCEMODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

namespace SOURCEMODULE
{

enum moduleconfigstate
{
    setDebugLevel,
};



class cSourceModuleConfigData;

const QString defaultXSDFile = "://src/sourcemodule.xsd";

// Sourcemoduleconfiguration holds configuration data as well as parameter

class cSourceModuleConfiguration: public cBaseModuleConfiguration
{
    Q_OBJECT
public:
    cSourceModuleConfiguration();
    ~cSourceModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cSourceModuleConfigData* getConfigurationData();

protected slots:
    virtual void configXMLInfo(QString key);
    virtual void completeConfiguration(bool ok);

private:
    cSourceModuleConfigData *m_pStatusModulConfigData;  // configuration
};

}

#endif // SOURCEMODULECONFIGURATION_H
