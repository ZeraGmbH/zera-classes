#ifndef SCPIMODULECONFIGURATION_H
#define SCPIMODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

namespace SCPIMODULE
{

enum moduleconfigstate
{
    setDebugLevel,
    setnrClient,
    setInterfacePort
};



class cSCPIModuleConfigData;

const QString defaultXSDFile = "://src/scpimodule.xsd";

// scpimoduleconfiguration holds configuration data

class cSCPIModuleConfiguration: public cBaseModuleConfiguration
{
    Q_OBJECT
public:
    cSCPIModuleConfiguration();
    ~cSCPIModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cSCPIModuleConfigData* getConfigurationData();

protected slots:
    virtual void configXMLInfo(QString key);
    virtual void completeConfiguration(bool ok);

private:
    // QHash<QString, VeinEntity*> m_exportEntityList; // we hold a list of all our entities for export
    cSCPIModuleConfigData *m_pSCPIModulConfigData;  // configuration
};

}

#endif // SCPIMODULCONFIGURATION_H
