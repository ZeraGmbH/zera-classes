#ifndef ADJMODULETESTHELPER_H
#define ADJMODULETESTHELPER_H

#include "modulemanagertestrunner.h"

class AdjModuleTestHelper
{
public:
    static void setActualTestValues(ModuleManagerTestRunner &testRunner,
                                    float testvoltage, float testcurrent, float testangle, float testfrequency);
};

#endif // ADJMODULETESTHELPER_H
