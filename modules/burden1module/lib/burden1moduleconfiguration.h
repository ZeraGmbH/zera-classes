#ifndef BURDEN1MODULECONFIGURATION_H
#define BURDEN1MODULECONFIGURATION_H

#include "basemoduleconfiguration.h"
#include "burden1moduleconfigdata.h"
#include <QByteArray>

namespace BURDEN1MODULE
{

class cBurden1ModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit cBurden1ModuleConfiguration(const QByteArray& xmlString);
    QByteArray exportConfiguration() const override;
    cBurden1ModuleConfigData* getConfigData();

private slots:
    void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);
private:
    void setConfiguration(const QByteArray& xmlString);
    cBurden1ModuleConfigData m_configData;
};

}

#endif // BURDEN1MODULCONFIGURATION_H
