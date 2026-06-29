#ifndef STATUSMODULECONFIGURATION_H
#define STATUSMODULECONFIGURATION_H

#include "basemoduleconfiguration.h"
#include "statusmoduleconfigdata.h"

namespace STATUSMODULE
{

class cStatusModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit cStatusModuleConfiguration(const QByteArray& xmlString);

    QByteArray exportConfiguration() const override;
    cStatusModuleConfigData* getConfigData();

private slots:
    void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);
private:
    void setConfiguration(const QByteArray& xmlString);
    cStatusModuleConfigData m_configData;
};

}

#endif // STATUSMODULCONFIGURATION_H
