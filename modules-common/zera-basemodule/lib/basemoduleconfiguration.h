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

    virtual QByteArray exportConfiguration() const = 0;
    bool isConfigured();

protected:
    bool m_bConfigured = false;
    bool m_bConfigError = false;
};

#endif // BASEMODULECONFIGURATION_H
