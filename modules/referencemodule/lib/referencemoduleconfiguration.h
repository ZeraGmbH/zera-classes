#ifndef REFERENCEMODULECONFIGURATION_H
#define REFERENCEMODULECONFIGURATION_H

#include "basemoduleconfiguration.h"
#include "referencemoduleconfigdata.h"

namespace REFERENCEMODULE
{

class cReferenceModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit cReferenceModuleConfiguration(const QByteArray& xmlString);
    QByteArray exportConfiguration() const override;
    cReferenceModuleConfigData* getConfigData();

private slots:
    void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);
private:
    void setConfiguration(const QByteArray& xmlString);
    cReferenceModuleConfigData m_configData;
};

}

#endif // REFERENCEMODULCONFIGURATION_H
