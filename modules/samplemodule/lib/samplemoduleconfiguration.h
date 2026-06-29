#ifndef SAMPLEMODULECONFIGURATION_H
#define SAMPLEMODULECONFIGURATION_H

#include "basemoduleconfiguration.h"
#include "samplemoduleconfigdata.h"

namespace SAMPLEMODULE
{

class cSampleModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit cSampleModuleConfiguration(const QByteArray& xmlString);

    QByteArray exportConfiguration() const override;
    cSampleModuleConfigData* getConfigData();

private slots:
    void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);
private:
    void setConfiguration(const QByteArray& xmlString);
    cSampleModuleConfigData m_configData;
};

}

#endif // SAMPLEMODULCONFIGURATION_H
