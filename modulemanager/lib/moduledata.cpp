#include "moduledata.h"

namespace ZeraModules {

ModuleData::ModuleData(VirtualModule *module, const QString &name, const QString &confPath, int moduleEntityId, int moduleNum) :
    m_module(module),
    m_uniqueName(name),
    m_configPath(confPath),
    m_moduleId(moduleEntityId),
    m_moduleNum(moduleNum)
{
}

ModuleData *ModuleData::findModuleByPointer(QList<ModuleData*> moduleDataList, VirtualModule *module)
{
    ModuleData *retVal = nullptr;
    for(ModuleData *tmpData : moduleDataList) {
        if(tmpData->m_module == module) {
            retVal = tmpData;
            break;
        }
    }
    return retVal;
}

}
