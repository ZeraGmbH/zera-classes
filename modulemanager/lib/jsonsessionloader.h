#ifndef JSONSESSIONLOADER_H
#define JSONSESSIONLOADER_H

#include <QObject>

class JsonSessionLoader : public QObject
{
    Q_OBJECT
public:
    explicit JsonSessionLoader(QObject *parent = nullptr);
    const QString &getSessionFilePath() const;
signals:
    void sigModuleInfoParsed(QString uniqueModuleName,
                       QString xmlPath,
                       int moduleEntityId,
                       int moduleNum);
public slots:
    void loadSession(QString filePath);
protected:
    static QString m_configDirName;

private:
    void parseModule(QJsonObject moduleObject);
    QString m_currentSessionFile;
};

#endif // JSONSESSIONLOADER_H
