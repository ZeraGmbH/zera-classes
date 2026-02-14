#ifndef SEM1MODULECONFIGURATION_H
#define SEM1MODULECONFIGURATION_H


#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "basemoduleconfiguration.h"

namespace SEM1MODULE
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



class cSem1ModuleConfigData;

// sem1moduleconfiguration holds configuration data as well as parameter

class cSem1ModuleConfiguration: public BaseModuleConfiguration
{
    Q_OBJECT
public:
    cSem1ModuleConfiguration();
    ~cSem1ModuleConfiguration();
    virtual void setConfiguration(const QByteArray& xmlString);
    virtual QByteArray exportConfiguration(); // exports conf. and parameters to xml
    cSem1ModuleConfigData* getConfigurationData();

protected slots:
    virtual void configXMLInfo(const QString &key);
    virtual void completeConfiguration(bool ok);

private:
    // QHash<QString, VeinEntity*> m_exportEntityList; // we hold a list of all our entities for export
    cSem1ModuleConfigData *m_pSem1ModulConfigData = nullptr;  // configuration
};

}

#endif // SEM1MODULCONFIGURATION_H
