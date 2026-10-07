#ifndef SEMSPMMODULE_H
#define SEMSPMMODULE_H

#include "basemeasmodule.h"
#include "basemeasprogram.h"
#include "semspmmoduleconfiguration.h"

class SemSpmModule : public cBaseMeasModule // this is the final target of cSem1Module / cSpm1Module
{
    Q_OBJECT
public:
    explicit SemSpmModule(const ModuleFactoryParam &moduleParam,
                          const QString &moduleDescription,
                          const QString &moduleName,
                          const QString &scpiModuleName);

    SemSpmModuleConfigData *getConfigData();
    QByteArray getConfigXml() const override;

    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

protected: // as long as cSem1Module / cSpm1Module exist
    cBaseMeasProgram *m_pMeasProgram = nullptr;

private:
    SemSpmModuleConfiguration m_configuration;
};

#endif // SEMSPMMODULE_H
