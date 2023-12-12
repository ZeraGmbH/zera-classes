#ifndef MODULEDATA_H
#define MODULEDATA_H

#include <virtualmodule.h>

namespace ZeraModules
{

class ModuleData
{
public:
    ModuleData(VirtualModule *t_ref, const QString &t_name, const QString &t_confPath, const QByteArray &t_confData, int moduleEntityId, int moduleNum);
    static ModuleData *findByReference(QList<ModuleData*> t_list, VirtualModule *t_ref);

    VirtualModule *m_reference;
    const QString m_uniqueName;
    const QString m_configPath;
    QByteArray m_configData;
    int m_moduleId;
    int m_moduleNum;
};

}
#endif // MODULEDATA_H
