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
    static QString getJsonStructurePathName(SupportedSourceTypes type);
    static QString getJsonStatePathName(SupportedSourceTypes type);
    static QString getJsonFileName(SupportedSourceTypes type);
};


class cSourceJsonStructureLoader
{
public:
    static QJsonObject getJsonStructure(QString fileName);
    static QJsonObject getJsonStructure(SupportedSourceTypes type);
};


#endif // CSOURCEJSONFILENAMEGETTER_H
