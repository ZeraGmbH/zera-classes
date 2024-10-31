#ifndef FFTMODULECONFIGURATION_H
#define FFTMODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

namespace FFTMODULE
{

enum moduleconfigstate
{
    setFftorder,
    setValueCount,
    setRefChannel,
    setMeasureInterval,
    setMovingwindowBool,
    setMovingwindowTime,

    setValue1 = 20
};



class cFftModuleConfigData;

// rangemoduleconfiguration holds configuration data as well as parameter

class cFftModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    cFftModuleConfiguration();
    ~cFftModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cFftModuleConfigData* getConfigurationData();

protected slots:
    virtual void configXMLInfo(QString key);
    virtual void completeConfiguration(bool ok);

private:
    cFftModuleConfigData *m_pFftModulConfigData;  // configuration
};

}

#endif // FFTMODULCONFIGURATION_H
