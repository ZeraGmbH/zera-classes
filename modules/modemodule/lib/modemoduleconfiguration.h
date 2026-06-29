#ifndef MODEMODULECONFIGURATION_H
#define MODEMODULECONFIGURATION_H

#include "basemoduleconfiguration.h"
#include "modemoduleconfigdata.h"

namespace MODEMODULE
{

class cModeModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit cModeModuleConfiguration(const QByteArray& xmlString);

    QByteArray exportConfiguration() const override;
    cModeModuleConfigData* getConfigData();

private slots:
    void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);
private:
    void setConfiguration(const QByteArray& xmlString);
    cModeModuleConfigData m_configData;
};

}

#endif // MODEMODULCONFIGURATION_H
