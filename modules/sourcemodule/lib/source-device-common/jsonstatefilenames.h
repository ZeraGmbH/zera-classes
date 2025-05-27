#ifndef JSONSTATEFILENAMES_H
#define JSONSTATEFILENAMES_H

#include <QString>

class JsonStateFilenames
{
public:
    static QString getJsonStatePath(const QString &deviceName, const QString &deviceVersion);
private:
    static QString getJsonStateDir();
    static void createDirIfNotExist(const QString &path);
};

#endif // JSONSTATEFILENAMES_H
