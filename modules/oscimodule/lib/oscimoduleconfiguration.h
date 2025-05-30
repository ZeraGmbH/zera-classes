#ifndef OSCIMODULECONFIGURATION_H
#define OSCIMODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

namespace OSCIMODULE
{

enum moduleconfigstate
{
    setInterpolation,
    setGap,
    setValueCount,
    setRefChannel,

    setValue1 = 20
};



class cOsciModuleConfigData;

// rangemoduleconfiguration holds configuration data as well as parameter

class cOsciModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    cOsciModuleConfiguration();
    ~cOsciModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cOsciModuleConfigData* getConfigurationData();

protected slots:
    virtual void configXMLInfo(QString key);
    virtual void completeConfiguration(bool ok);

private:
    cOsciModuleConfigData *m_pOsciModulConfigData = nullptr;  // configuration
};

}

#endif // OSCIMODULCONFIGURATION_H
