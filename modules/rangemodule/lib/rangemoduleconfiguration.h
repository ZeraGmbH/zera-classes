#ifndef RANGEMODULECONFIGURATION_H
#define RANGEMODULECONFIGURATION_H

#include "basemoduleconfiguration.h"
#include "rangemoduleconfigdata.h"

namespace RANGEMODULE
{

class cRangeModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit cRangeModuleConfiguration(const QByteArray& xmlString);

    QByteArray exportConfiguration() const override;
    cRangeModuleConfigData* getConfigData();

private slots:
    virtual void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);
private:
    void setConfiguration(const QByteArray& xmlString);
    cRangeModuleConfigData m_configData;
};

}

#endif // RANGEMODULCONFIGURATION_H
