#ifndef RMSMODULECONFIGURATION_H
#define RMSMODULECONFIGURATION_H

#include "basemoduleconfiguration.h"
#include "rmsmoduleconfigdata.h"

namespace RMSMODULE
{

class cRmsModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit cRmsModuleConfiguration(const QByteArray& xmlString);
    QByteArray exportConfiguration() const override;
    cRmsModuleConfigData* getConfigData();

private slots:
    void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);
private:
    void setConfiguration(const QByteArray& xmlString);
    cRmsModuleConfigData m_configData;
};

}

#endif // RMSMODULCONFIGURATION_H
