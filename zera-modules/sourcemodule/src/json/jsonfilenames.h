#ifndef JSONFILENAMES_H
#define JSONFILENAMES_H

#include "source-device/supportedsources.h"
#include <QString>

class JsonFilenames
{
public:
public:
    static QString getJsonStructurePath(SupportedSourceTypes type);
    static QString getJsonStatePath(SupportedSourceTypes type);
    static QString getJsonStatePath(QString deviceName, QString deviceVersion);
    static QString getJsonFileName(SupportedSourceTypes type);
private:
    static QString getJsonStateDir();
    static void createDirIfNotExist(QString path);
};

#endif // JSONFILENAMES_H
