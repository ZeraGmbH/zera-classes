#ifndef SYSINFO_H
#define SYSINFO_H

#include <QString>

class SysInfo
{
public:
    static QString getSerialNr();
    static QString getReleaseNr();
};

#endif // SYSINFO_H
