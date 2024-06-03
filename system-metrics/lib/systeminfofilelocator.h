#ifndef SYSTEMINFOFILELOCATOR_H
#define SYSTEMINFOFILELOCATOR_H

#include <QString>

class SystemInfoFileLocator
{
public:
    static QString getProcStatusFileName();
protected:
    static QString m_procStatusFileName;
};

#endif // SYSTEMINFOFILELOCATOR_H
