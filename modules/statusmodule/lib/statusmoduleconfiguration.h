#ifndef STATUSMODULECONFIGURATION_H
#define STATUSMODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

namespace STATUSMODULE
{

enum moduleconfigstate
{
    setMeasStatus,
    setAccumulator
};

class cStatusModuleConfigData;

// Statusmoduleconfiguration holds configuration data as well as parameter
class cStatusModuleConfiguration: public cBaseModuleConfiguration
{
    Q_OBJECT
public:
    cStatusModuleConfiguration();
    ~cStatusModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cStatusModuleConfigData* getConfigurationData();
protected slots:
    virtual void configXMLInfo(QString key);
    virtual void completeConfiguration(bool ok);
private:
    void validateAndSetConfig(QByteArray xmlString, QString xsdFilename);
    cStatusModuleConfigData *m_pStatusModulConfigData;  // configuration
};

}

#endif // STATUSMODULCONFIGURATION_H
