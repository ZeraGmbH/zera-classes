#ifndef MODULECONFIGURATIONNULL_H
#define MODULECONFIGURATIONNULL_H

#include "basemoduleconfiguration.h"

class ModuleConfigurationNull : public BaseModuleConfiguration
{
    Q_OBJECT
public:
    ModuleConfigurationNull();
    virtual void setConfiguration(const QByteArray& xmlString) override;
    virtual QByteArray exportConfiguration() override;
    virtual void configXMLInfo(const QString &key) override;
};

#endif // MODULECONFIGURATIONNULL_H
