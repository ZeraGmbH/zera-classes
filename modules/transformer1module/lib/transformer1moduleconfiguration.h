#ifndef TRANSFORMER1MODULECONFIGURATION_H
#define TRANSFORMER1MODULECONFIGURATION_H

#include "basemoduleconfiguration.h"
#include "transformer1moduleconfigdata.h"

namespace TRANSFORMER1MODULE
{
class cTransformer1ModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit cTransformer1ModuleConfiguration(const QByteArray& xmlString);

    QByteArray exportConfiguration() const override;
    cTransformer1ModuleConfigData* getConfigData();

private slots:
    virtual void configXMLInfo(const QString &key) override;
    virtual void completeConfiguration(bool ok);
private:
    void setConfiguration(const QByteArray& xmlString);
    cTransformer1ModuleConfigData m_configData;
};

}

#endif // TRANSFORMER1MODULCONFIGURATION_H
