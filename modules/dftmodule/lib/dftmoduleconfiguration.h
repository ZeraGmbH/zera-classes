#ifndef DFTMODULECONFIGURATION_H
#define DFTMODULECONFIGURATION_H

#include "basemoduleconfiguration.h"
#include "dftmoduleconfigdata.h"

namespace DFTMODULE
{

class cDftModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit cDftModuleConfiguration(const QByteArray& xmlString);

    QByteArray exportConfiguration() const override;
    cDftModuleConfigData* getConfigData();

private slots:
    virtual void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);

private:
    void setConfiguration(const QByteArray& xmlString);
    cDftModuleConfigData m_configData;
};

}

#endif // DFTMODULCONFIGURATION_H
