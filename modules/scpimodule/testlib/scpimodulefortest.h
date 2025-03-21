#ifndef SCPIMODULEFORTEST_H
#define SCPIMODULEFORTEST_H

#include "scpimodule.h"
#include "scpimoduleconfigdata.h"
#include "scpiinterface.h"

namespace SCPIMODULE {

class ScpiModuleForTest : public cSCPIModule
{
    Q_OBJECT
public:
    ScpiModuleForTest(ModuleFactoryParam moduleParam);
    cSCPIModuleConfigData *getConfigData() const;
    cSCPIInterface* getScpiInterface() const;
};

}

#endif // SCPIMODULEFORTEST_H
