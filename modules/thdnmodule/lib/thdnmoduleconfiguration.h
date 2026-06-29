#ifndef THDNMODULECONFIGURATION_H
#define THDNMODULECONFIGURATION_H

#include "basemoduleconfiguration.h"
#include "thdnmoduleconfigdata.h"
#include <QByteArray>
#include <QHash>

namespace THDNMODULE
{

class cThdnModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    explicit cThdnModuleConfiguration(const QByteArray& xmlString);

    QByteArray exportConfiguration() const override;
    cThdnModuleConfigData* getConfigData();

protected slots:
    virtual void configXMLInfo(const QString &key) override;
    virtual void completeConfiguration(bool ok);

private:
    void setConfiguration(const QByteArray& xmlString);
    cThdnModuleConfigData m_configData;
};

}

#endif // THDNMODULCONFIGURATION_H
