#ifndef THDNMODULECONFIGURATION_H
#define THDNMODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

namespace THDNMODULE
{

class cThdnModuleConfigData;

class cThdnModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    cThdnModuleConfiguration();
    ~cThdnModuleConfiguration();
    virtual void setConfiguration(const QByteArray& xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cThdnModuleConfigData* getConfigurationData();

protected slots:
    virtual void configXMLInfo(const QString &key);
    virtual void completeConfiguration(bool ok);

private:
    cThdnModuleConfigData *m_pThdnModulConfigData = nullptr;  // configuration
};

}

#endif // THDNMODULCONFIGURATION_H
