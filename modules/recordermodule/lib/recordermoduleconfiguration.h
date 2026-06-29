#ifndef RECORDERMODULECONFIGURATION_H
#define RECORDERMODULECONFIGURATION_H

#include "recordermoduleconfigdata.h"
#include <basemoduleconfiguration.h>

class RecorderModuleConfiguration : public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit RecorderModuleConfiguration(const QByteArray& xmlString);

    virtual QByteArray exportConfiguration() const override;
    RecorderModuleConfigData* getConfigData();

private slots:
    void configXMLInfo(const QString &key) override;
    virtual void completeConfiguration(bool ok);
private:
    void setConfiguration(const QByteArray& xmlString);
    RecorderModuleConfigData m_configData;
};

#endif // RECORDERMODULECONFIGURATION_H
