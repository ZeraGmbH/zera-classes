#ifndef LAMBDAMODULECONFIGURATION_H
#define LAMBDAMODULECONFIGURATION_H

#include "basemoduleconfiguration.h"
#include "lambdamoduleconfigdata.h"
#include <QByteArray>

namespace LAMBDAMODULE
{

class cLambdaModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit cLambdaModuleConfiguration(const QByteArray& xmlString);

    QByteArray exportConfiguration() const override;
    cLambdaModuleConfigData* getConfigData();

private slots:
    void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);
private:
    void setConfiguration(const QByteArray& xmlString);
    cLambdaModuleConfigData m_configData;
};

}

#endif // LAMBDAMODULCONFIGURATION_H
