#ifndef CSOURCEJSONFILENAMEGETTER_H
#define CSOURCEJSONFILENAMEGETTER_H

#include <QString>
#include <QJsonObject>

enum SupportedSourceTypes {
    SOURCE_MT_COMMON = 0,  // 3ph common
    SOURCE_MT_CURRENT_ONLY,
    SOURCE_MT_SINGLE_PHASE_ONLY,

    SOURCE_DEMO_FG_4PHASE, // AUX/harmonics

    SOURCE_TYPE_COUNT
};


class cSourceJsonFilenames
{
public:
    static QString getJsonStructurePath(SupportedSourceTypes type);
    static QString getJsonStateDir();
    static QString getJsonStatePath(SupportedSourceTypes type);
    static QString getJsonStatePath(QString deviceName, QString deviceVersion);
    static QString getJsonFileName(SupportedSourceTypes type);
public:
    static void createDirIfNotExist(QString path);
};


class cSourceJsonStructureLoader
{
public:
    static QJsonObject getJsonStructure(QString fileName);
    static QJsonObject getJsonStructure(SupportedSourceTypes type);
};


#endif // CSOURCEJSONFILENAMEGETTER_H
