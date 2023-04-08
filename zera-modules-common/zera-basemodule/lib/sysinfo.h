#ifndef SYSINFO_H
#define SYSINFO_H

#include <QString>

class SysInfo
{
public:
    static QString getSerialNr(QString path);
    static QString getReleaseNr(QString path);
};

#endif // SYSINFO_H
