#ifndef SOURCEMODULECONFIGURATION_H
#define SOURCEMODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include <basemoduleconfiguration.h>
#include <sourcemodule-config.hxx>

// Sourcemoduleconfiguration holds module configuration

class SourceModuleConfiguration: public cBaseModuleConfiguration
{
    Q_OBJECT
public:
    SourceModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString) override;
    virtual QByteArray exportConfiguration() override; // exports conf. and parameters to xml
    configuration* getConfigXMLWrapper();

protected slots: // for the sake of XMLSettings - we introduced m_xsdGeneratedConfig
    virtual void configXMLInfo(QString) override {};

private:
    std::unique_ptr<configuration> m_xsdGeneratedConfig; // configuration
};

#endif // SOURCEMODULECONFIGURATION_H
