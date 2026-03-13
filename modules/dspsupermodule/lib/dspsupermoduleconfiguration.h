#ifndef DSPSUPERMODULECONFIGURATION_H
#define DSPSUPERMODULECONFIGURATION_H

#include "dspsupermoduleconfigdata.h"
#include "basemoduleconfiguration.h"
#include <QByteArray>

namespace DSPSUPERMODULE
{

class DspSuperModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    DspSuperModuleConfiguration();
    ~DspSuperModuleConfiguration();
    void setConfiguration(const QByteArray &xmlString) override;
    virtual QByteArray exportConfiguration() override; // exports conf. and parameters to xml
    DspSuperModuleConfigData* getConfigurationData();

private slots:
    void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);
private:
    DspSuperModuleConfigData *m_dspSuperModulConfigData = nullptr;
};

}

#endif // DSPSUPERMODULECONFIGURATION_H
