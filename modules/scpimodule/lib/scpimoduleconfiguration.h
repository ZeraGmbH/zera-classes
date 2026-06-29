#ifndef SCPIMODULECONFIGURATION_H
#define SCPIMODULECONFIGURATION_H

#include "basemoduleconfiguration.h"
#include "scpimoduleconfigdata.h"

namespace SCPIMODULE
{

class cSCPIModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit cSCPIModuleConfiguration(const QByteArray& xmlString);

    QByteArray exportConfiguration() const override;
    cSCPIModuleConfigData* getConfigData();

private slots:
    void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);
private:
    void setConfiguration(const QByteArray& xmlString);
    cSCPIModuleConfigData m_configData;
};

}

#endif // SCPIMODULCONFIGURATION_H
