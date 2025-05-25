#ifndef APIMODULECONFIGURATION_H
#define APIMODULECONFIGURATION_H

#include "apimoduleconfigdata.h"
#include "basemoduleconfiguration.h"

namespace APIMODULE
{

    enum moduleconfigstate
    {
    };

    // moduleconfiguration holds configuration data as well as parameter

    class cApiModuleConfiguration : public BaseModuleConfiguration
    {
        Q_OBJECT
    public:
        cApiModuleConfiguration();
        ~cApiModuleConfiguration();
        virtual void setConfiguration(QByteArray xmlString);
        virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
        cApiModuleConfigData *getConfigurationData();

    protected slots:
        virtual void configXMLInfo(QString key);
        virtual void completeConfiguration(bool ok);

    private:
        cApiModuleConfigData *m_pApiModulConfigData = nullptr; // configuration
    };

}

#endif // APIMODULECONFIGURATION_H
