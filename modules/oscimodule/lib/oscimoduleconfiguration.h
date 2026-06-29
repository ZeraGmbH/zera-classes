#ifndef OSCIMODULECONFIGURATION_H
#define OSCIMODULECONFIGURATION_H

#include "basemoduleconfiguration.h"
#include "oscimoduleconfigdata.h"

namespace OSCIMODULE
{

class cOsciModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit cOsciModuleConfiguration(const QByteArray& xmlString);

    QByteArray exportConfiguration() const override;
    cOsciModuleConfigData* getConfigData();

private slots:
    void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);
private:
    void setConfiguration(const QByteArray& xmlString);
    cOsciModuleConfigData m_configData;
};

}

#endif // OSCIMODULCONFIGURATION_H
