#ifndef MODULECONFIGURATIONNULL_H
#define MODULECONFIGURATIONNULL_H

#include "basemoduleconfiguration.h"

class ModuleConfigurationNull : public BaseModuleConfiguration
{
    Q_OBJECT
public:
    ModuleConfigurationNull();
    virtual void setConfiguration(QByteArray xmlString) override;
    virtual QByteArray exportConfiguration() override;
    virtual void configXMLInfo(QString key) override;
};

#endif // MODULECONFIGURATIONNULL_H
