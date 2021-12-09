#ifndef CSOURCEJSONFILENAMEGETTER_H
#define CSOURCEJSONFILENAMEGETTER_H

#include <QString>
#include <QJsonObject>

enum SupportedSourceTypes {
    SOURCE_MT_COMMON = 0,  // basic 3ph
    SOURCE_MT400_20,       // current only
    SOURCE_MT507,          // single phase

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


class cSourcePersistentJsonState
{
public:
    cSourcePersistentJsonState(SupportedSourceTypes type);

    QJsonObject getJsonStructure();

    QJsonObject loadJsonState();
    void saveJsonState(QJsonObject state);
private:
    SupportedSourceTypes m_sourceType;
    QJsonObject m_jsonParamStructure;
};

#endif // CSOURCEJSONFILENAMEGETTER_H
