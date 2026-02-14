#ifndef RECORDERMODULECONFIGURATION_H
#define RECORDERMODULECONFIGURATION_H

#include <basemoduleconfiguration.h>

class RecorderModuleConfigData;

class RecorderModuleConfiguration : public BaseModuleConfiguration
{
    Q_OBJECT
public:
    RecorderModuleConfiguration();
    ~RecorderModuleConfiguration();
    virtual void setConfiguration(const QByteArray& xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    RecorderModuleConfigData* getConfigurationData();
protected slots:
    virtual void configXMLInfo(const QString &key);
    virtual void completeConfiguration(bool ok);
private:
    RecorderModuleConfigData *m_pRecorderModuleConfigData = nullptr;  // configuration
};

#endif // RECORDERMODULECONFIGURATION_H
