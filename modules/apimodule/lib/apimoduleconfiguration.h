#ifndef APIMODULECONFIGURATION_H
#define APIMODULECONFIGURATION_H

#include "basemoduleconfiguration.h"
#include "apimoduleconfigdata.h"

namespace APIMODULE
{

class cApiModuleConfiguration : public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit cApiModuleConfiguration(const QByteArray& xmlString);

    QByteArray exportConfiguration() const override;
    cApiModuleConfigData *getConfigData();

protected slots:
    void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);

private:
    void setConfiguration(const QByteArray& xmlString);
    cApiModuleConfigData m_configData;
};

}

#endif // APIMODULECONFIGURATION_H
