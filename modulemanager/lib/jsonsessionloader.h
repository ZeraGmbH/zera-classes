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
    void sigModuleInfoParsed(const QString &uniqueModuleName,
                             const QString &xmlPath,
                             int moduleEntityId,
                             int moduleNum,
                             bool licensable);
public slots:
    void loadSession(const QString &filePath);
protected:
    static QString m_configDirName;

private:
    void parseModule(const QJsonObject &moduleObject);
    QString m_currentSessionFile;
};

#endif // JSONSESSIONLOADER_H
