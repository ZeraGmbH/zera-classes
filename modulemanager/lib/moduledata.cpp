#include "moduledata.h"

namespace ZeraModules {

ModuleData::ModuleData(VirtualModule *module, const QString &name, const QString &confPath, int moduleEntityId, int moduleNum, bool licensable) :
    m_module(module),
    m_uniqueName(name),
    m_configPath(confPath),
    m_moduleId(moduleEntityId),
    m_moduleNum(moduleNum),
    m_licensable(licensable)
{
}

}
