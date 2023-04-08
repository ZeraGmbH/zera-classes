#ifndef ELECTRONICBURDENMODULECONFIGURATION_H
#define ELECTRONICBURDENMODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include <basemoduleconfiguration.h>
#include <electronicburdenmodule-config.hxx>

namespace ELECTRONICBURDENMODULE
{

// Elctronicburdenconfiguration holds module configuration

class ElectronicBurdenModuleConfiguration: public cBaseModuleConfiguration
{
    Q_OBJECT
public:
    ElectronicBurdenModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString) override;
    virtual QByteArray exportConfiguration() override; // exports conf. and parameters to xml
    configuration* getConfigXMLWrapper();

protected slots: // for the sake of XMLSettings - we introduced m_xsdGeneratedConfig
    virtual void configXMLInfo(QString) override {};

private:
    std::unique_ptr<configuration> m_xsdGeneratedConfig; // configuration
};

}

#endif // ELECTRONICBURDENMODULECONFIGURATION_H
