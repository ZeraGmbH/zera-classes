#include "moduledata.h"

namespace ZeraModules {

ModuleData::ModuleData(VirtualModule *ref, const QString &name, const QString &confPath, const QByteArray &confData, int moduleEntityId, int moduleNum) :
    m_reference(ref),
    m_uniqueName(name),
    m_configPath(confPath),
    m_configData(confData),
    m_moduleId(moduleEntityId),
    m_moduleNum(moduleNum)
{
}

ModuleData *ModuleData::findByReference(QList<ModuleData*> list, VirtualModule *ref)
{
    ModuleData *retVal = nullptr;
    foreach(ModuleData *tmpData, list) {
        if(tmpData->m_reference == ref) {
            retVal = tmpData;
            break;
        }
    }
    return retVal;
}

}
