#ifndef STATUSMODULECONFIGURATION_H
#define STATUSMODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

namespace STATUSMODULE
{


class cStatusModuleConfigData;

class cStatusModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    cStatusModuleConfiguration();
    ~cStatusModuleConfiguration();
    virtual void setConfiguration(const QByteArray& xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cStatusModuleConfigData* getConfigurationData();
protected slots:
    virtual void configXMLInfo(const QString &key);
    virtual void completeConfiguration(bool ok);
private:
    cStatusModuleConfigData *m_pStatusModulConfigData = nullptr;  // configuration
};

}

#endif // STATUSMODULCONFIGURATION_H
