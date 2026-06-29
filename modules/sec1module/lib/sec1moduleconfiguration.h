#ifndef SEC1MODULECONFIGURATION_H
#define SEC1MODULECONFIGURATION_H

#include "basemoduleconfiguration.h"
#include "sec1moduleconfigdata.h"

namespace SEC1MODULE
{

class cSec1ModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit cSec1ModuleConfiguration(const QByteArray& xmlString);

    QByteArray exportConfiguration() const override;
    cSec1ModuleConfigData* getConfigData();

private slots:
    void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);

private:
    void setConfiguration(const QByteArray& xmlString);
    cSec1ModuleConfigData m_configData;
};

}

#endif // SEC1MODULCONFIGURATION_H
