#ifndef DSPSUPERMODULECONFIGURATION_H
#define DSPSUPERMODULECONFIGURATION_H

#include "basemoduleconfiguration.h"
#include "dspsupermoduleconfigdata.h"

namespace DSPSUPERMODULE
{

class DspSuperModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit DspSuperModuleConfiguration(const QByteArray &xmlString);

    QByteArray exportConfiguration() const override;
    DspSuperModuleConfigData* getConfigData();

private slots:
    void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);
private:
    void setConfiguration(const QByteArray &xmlString);
    DspSuperModuleConfigData m_configData;
};

}

#endif // DSPSUPERMODULECONFIGURATION_H
