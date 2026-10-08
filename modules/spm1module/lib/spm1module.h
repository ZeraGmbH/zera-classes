#ifndef SPM1MODULE_H
#define SPM1MODULE_H

#include "semspmmodule.h"

namespace SPM1MODULE
{
class cSpm1Module : public SemSpmModule
{
    Q_OBJECT
public:
    explicit cSpm1Module(const ModuleFactoryParam &moduleParam,
                         const QString &moduleDescription,
                         const QString &moduleName,
                         const QString &scpiModuleName);

private:
    void setupModule() override; // after xml configuration we can setup and export our module
};

}

#endif // SPM1MODULE_H
