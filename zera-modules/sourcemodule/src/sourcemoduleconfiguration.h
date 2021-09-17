#ifndef SOURCEMODULECONFIGURATION_H
#define SOURCEMODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include <basemoduleconfiguration.h>
#include <sourcemodule-config.hxx>

namespace SOURCEMODULE
{

//const QString defaultXSDFile = "://src/sourcemodule-config.xsd";

// Sourcemoduleconfiguration holds configuration data as well as parameter

class cSourceModuleConfiguration: public cBaseModuleConfiguration
{
    Q_OBJECT
public:
    cSourceModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    configuration* getConfigXMLWrapper();

protected slots: // for the sake of cXMLSettings - we introduced m_xsdGeneratedConfig
    virtual void configXMLInfo(QString key) {};

private:
    std::unique_ptr<configuration> m_xsdGeneratedConfig; // configuration
};

}

#endif // SOURCEMODULECONFIGURATION_H
