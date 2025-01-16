#ifndef ADJMODULETESTHELPER_H
#define ADJMODULETESTHELPER_H

#include "modulemanagertestrunner.h"

class AdjModuleTestHelper
{
public:
    static void setActualTestValues(ModuleManagerTestRunner &testRunner,
                                    float testvoltage, float testcurrent, float testangle, float testfrequency);
    struct TAdjNodeValues
    {
        double m_loadPoint = 0.0;
        double m_correction = 0.0;
    };
    static TAdjNodeValues parseNode(const QString &scpiResponse);
};

#endif // ADJMODULETESTHELPER_H
