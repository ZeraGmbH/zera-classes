#ifndef MODULEMANAGERCONFIGTEST_H
#define MODULEMANAGERCONFIGTEST_H

#include <modulemanagerconfig.h>

class ModulemanagerConfigTest : public ModulemanagerConfig
{
public:
    static void enableTest();
    static void setConfigFile(QString configFile);
};

#endif // MODULEMANAGERCONFIGTEST_H
