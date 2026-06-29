#ifndef SEM1MODULECONFIGURATION_H
#define SEM1MODULECONFIGURATION_H

#include "basemoduleconfiguration.h"
#include "sem1moduleconfigdata.h"

namespace SEM1MODULE
{

class cSem1ModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit cSem1ModuleConfiguration(const QByteArray& xmlString);

    QByteArray exportConfiguration() const override;
    cSem1ModuleConfigData* getConfigData();

private slots:
    void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);
private:
    void setConfiguration(const QByteArray& xmlString);
    cSem1ModuleConfigData m_configData;
};

}

#endif // SEM1MODULCONFIGURATION_H
