#ifndef SOURCEMODULECONFIGURATION_H
#define SOURCEMODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include <basemoduleconfiguration.h>
#include <sourcemodule-config.hxx>

namespace SOURCEMODULE
{

// Sourcemoduleconfiguration holds module configuration

class cSourceModuleConfiguration: public cBaseModuleConfiguration
{
    Q_OBJECT
public:
    cSourceModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString) override;
    virtual QByteArray exportConfiguration() override; // exports conf. and parameters to xml
    configuration* getConfigXMLWrapper();

protected slots: // for the sake of cXMLSettings - we introduced m_xsdGeneratedConfig
    virtual void configXMLInfo(QString) override {};

private:
    std::unique_ptr<configuration> m_xsdGeneratedConfig; // configuration
};

}

#endif // SOURCEMODULECONFIGURATION_H
