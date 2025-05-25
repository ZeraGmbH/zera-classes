#ifndef SPM1MODULECONFIGURATION_H
#define SPM1MODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

namespace SPM1MODULE
{

enum moduleconfigstate
{
    setRefInputCount,
    setActiveUnitCount,
    setReactiveUnitCount,
    setApparentUnitCount,

    setRefInput,
    setTargeted,
    setMeasTime,
    setUpperLimit,
    setLowerLimit,

    setRefInput1Name = 32,
    setActiveUnit1Name = 64,
    setReactiveUnit1Name = 80,
    setApparentUnit1Name = 96,
    setRefInput1Append = 128
};



class cSpm1ModuleConfigData;

// spm1moduleconfiguration holds configuration data as well as parameter

class cSpm1ModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    cSpm1ModuleConfiguration();
    ~cSpm1ModuleConfiguration();
    virtual void setConfiguration(QByteArray xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cSpm1ModuleConfigData* getConfigurationData();

protected slots:
    virtual void configXMLInfo(QString key);
    virtual void completeConfiguration(bool ok);

private:
    // QHash<QString, VeinEntity*> m_exportEntityList; // we hold a list of all our entities for export
    cSpm1ModuleConfigData *m_pSpm1ModulConfigData = nullptr;  // configuration
};

}

#endif // SPM1MODULCONFIGURATION_H
