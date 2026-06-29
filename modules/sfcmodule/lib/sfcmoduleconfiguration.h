#ifndef SFCMODULECONFIGURATION_H
#define SFCMODULECONFIGURATION_H

#include "basemoduleconfiguration.h"
#include "sfcmoduleconfigdata.h"

namespace SFCMODULE
{

class cSfcModuleConfiguration : public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit cSfcModuleConfiguration(const QByteArray& xmlString);
    QByteArray exportConfiguration() const override;
    cSfcModuleConfigData *getConfigData();

private slots:
    void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);
private:
    void setConfiguration(const QByteArray& xmlString);
    cSfcModuleConfigData m_configData;
};

}

#endif // SFCMODULECONFIGURATION_H
