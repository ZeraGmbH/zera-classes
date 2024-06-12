#ifndef SYSTEMINFOFILELOCATOR_H
#define SYSTEMINFOFILELOCATOR_H

#include <QString>

class SystemInfoFileLocator
{
public:
    static QString getProcStatusFileName();
    static QString getProcMeminfoFileName();
protected:
    static QString m_procStatusFileName;
    static QString m_procMeminfoFileName;
};

#endif // SYSTEMINFOFILELOCATOR_H
