#ifndef MODULECONFIGURATIONNULL_H
#define MODULECONFIGURATIONNULL_H

#include "basemoduleconfiguration.h"

class ModuleConfigurationNull : public BaseModuleConfiguration
{
    Q_OBJECT
public:
    ModuleConfigurationNull();
    QByteArray exportConfiguration() const override;
    void configXMLInfo(const QString &key) override;
};

#endif // MODULECONFIGURATIONNULL_H
