#ifndef SEM1MODULE_H
#define SEM1MODULE_H

#include "semspmmodule.h"

namespace SEM1MODULE
{
class cSem1Module : public SemSpmModule
{
    Q_OBJECT
public:
    explicit cSem1Module(const ModuleFactoryParam &moduleParam,
                         const QString &moduleDescription,
                         const QString &moduleName,
                         const QString &scpiModuleName);

private:
    void setupModule() override; // after xml configuration we can setup and export our module
};

}

#endif // SEM1MODULE_H
