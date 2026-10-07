#ifndef SPM1MODULECONFIGURATION_H
#define SPM1MODULECONFIGURATION_H

#include "basemoduleconfiguration.h"
#include "semspmmoduleconfigdata.h"

namespace SPM1MODULE
{

class cSpm1ModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit cSpm1ModuleConfiguration(const QByteArray& xmlString);

    QByteArray exportConfiguration() const override;
    SemSpmModuleConfigData* getConfigData();

private slots:
    void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);
private:
    void setConfiguration(const QByteArray& xmlString);
    SemSpmModuleConfigData m_configData;
};

}

#endif // SPM1MODULCONFIGURATION_H
