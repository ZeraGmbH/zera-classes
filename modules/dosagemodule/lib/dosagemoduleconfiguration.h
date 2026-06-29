#ifndef DOSAGEMODULECONFIGURATION_H
#define DOSAGEMODULECONFIGURATION_H

#include "basemoduleconfiguration.h"
#include "dosagemoduleconfigdata.h"

namespace DOSAGEMODULE
{

class cDosageModuleConfiguration : public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit cDosageModuleConfiguration(const QByteArray& xmlString);

    QByteArray exportConfiguration() const override;
    cDosageModuleConfigData *getConfigData();

protected slots:
    void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);

private:
    void setConfiguration(const QByteArray& xmlString);

    cDosageModuleConfigData m_configData;
};

}

#endif // DOSAGEMODULECONFIGURATION_H
