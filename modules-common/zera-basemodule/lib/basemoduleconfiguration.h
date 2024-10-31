#ifndef BASEMODULECONFIGURATION_H
#define BASEMODULECONFIGURATION_H

#include <xmlsettings.h>

// pure virtual base class for module configuration

class BaseModuleConfiguration: public XMLSettings
{
    Q_OBJECT
public:
    BaseModuleConfiguration();
    virtual ~BaseModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString) = 0;
    virtual QByteArray exportConfiguration() = 0; // exports conf. and parameters to xml
    bool isConfigured();
protected:
    bool m_bConfigured;
    bool m_bConfigError;
};

#endif // BASEMODULECONFIGURATION_H
