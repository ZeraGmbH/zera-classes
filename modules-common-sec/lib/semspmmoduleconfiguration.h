#ifndef SEMSPMMODULECONFIGURATION_H
#define SEMSPMMODULECONFIGURATION_H

#include "basemoduleconfiguration.h"
#include "semspmmoduleconfigdata.h"

class SemSpmModuleConfiguration : public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit SemSpmModuleConfiguration(const QByteArray& xmlString);

    QByteArray exportConfiguration() const override;
    SemSpmModuleConfigData* getConfigData();

private slots:
    void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);
private:
    void setConfiguration(const QByteArray& xmlString);
    SemSpmModuleConfigData m_configData;
};

#endif // SEMSPMMODULECONFIGURATION_H
