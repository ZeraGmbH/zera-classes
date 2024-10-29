#ifndef MODULEDATA_H
#define MODULEDATA_H

#include <virtualmodule.h>

namespace ZeraModules
{

class ModuleData
{
public:
    ModuleData(VirtualModule *ref, const QString &name, const QString &confPath, const QByteArray &confData, int moduleEntityId, int moduleNum);
    static ModuleData *findByReference(QList<ModuleData*> list, VirtualModule *ref);

    VirtualModule *m_reference;
    const QString m_uniqueName;
    const QString m_configPath;
    QByteArray m_configData;
    int m_moduleId;
    int m_moduleNum;
};

}
#endif // MODULEDATA_H
