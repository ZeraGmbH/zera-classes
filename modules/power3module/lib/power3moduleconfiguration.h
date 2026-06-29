#ifndef POWER3MODULECONFIGURATION_H
#define POWER3MODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"
#include "power3moduleconfigdata.h"

namespace POWER3MODULE
{

class cPower3ModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit cPower3ModuleConfiguration(const QByteArray& xmlString);

    QByteArray exportConfiguration() const override;
    cPower3ModuleConfigData* getConfigData();

private slots:
    void configXMLInfo(const QString &key) override;
    void completeConfiguration(bool ok);

private:
    void setConfiguration(const QByteArray& xmlString);
    cPower3ModuleConfigData m_configData;
};

}

#endif // POWER3MODULCONFIGURATION_H
