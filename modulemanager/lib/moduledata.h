#ifndef MODULEDATA_H
#define MODULEDATA_H

#include <virtualmodule.h>

namespace ZeraModules
{

class ModuleData
{
public:
    ModuleData(VirtualModule *module, const QString &name, const QString &confPath, int moduleEntityId, int moduleNum);

    VirtualModule *m_module;
    const QString m_uniqueName;
    const QString m_configPath;
    const int m_moduleId;
    const int m_moduleNum;
};

}
#endif // MODULEDATA_H
