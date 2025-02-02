#ifndef ADJMODULETESTHELPER_H
#define ADJMODULETESTHELPER_H

#include "modulemanagertestrunner.h"

class AdjModuleTestHelper
{
public:
    static void setActualTestValues(ModuleManagerTestRunner &testRunner,
                                    float testvoltage, float testcurrent, float testangle, float testfrequency);
    static void setAllValuesSymmetricAc(ModuleManagerTestRunner &testRunner,
                                        float testvoltage, float testcurrent, float testangle, float testfrequency);
    static void setAllValuesSymmetricDc(ModuleManagerTestRunner &testRunner,
                                        float voltage, float current);
    struct TAdjNodeValues
    {
        double m_loadPoint = 0.0;
        double m_correction = 0.0;
    };
    static TAdjNodeValues parseNode(const QString &scpiResponse);
};

#endif // ADJMODULETESTHELPER_H
