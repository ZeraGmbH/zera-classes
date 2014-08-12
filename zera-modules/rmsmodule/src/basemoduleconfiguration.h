#ifndef BASEMODULECONFIGURATION_H
#define BASEMODULECONFIGURATION_H

#include "xmlsettings.h"

namespace RMSMODULE
{

// pure virtual base class for module configuration

class cBaseModuleConfiguration: public cXMLSettings
{
    Q_OBJECT

public:
    cBaseModuleConfiguration();
    virtual ~cBaseModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString) = 0;
    virtual QByteArray exportConfiguration() = 0; // exports conf. and parameters to xml
    bool isConfigured();

signals:
    void configXMLDone();

protected:
    bool m_bConfigured;
    bool m_bConfigError;
};

}
#endif // BASEMODULECONFIGURATION_H
