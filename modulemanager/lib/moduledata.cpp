#include "moduledata.h"

namespace ZeraModules {

ModuleData::ModuleData(VirtualModule *t_ref, const QString &t_name, const QString &t_confPath, const QByteArray &t_confData, int moduleEntityId) :
    m_reference(t_ref),
    m_uniqueName(t_name),
    m_configPath(t_confPath),
    m_configData(t_confData),
    m_moduleId(moduleEntityId)
{
}

ModuleData *ModuleData::findByReference(QList<ModuleData*> t_list, VirtualModule *t_ref)
{
    ModuleData *retVal = nullptr;
    foreach(ModuleData *tmpData, t_list) {
        if(tmpData->m_reference == t_ref) {
            retVal = tmpData;
            break;
        }
    }
    return retVal;
}

}
