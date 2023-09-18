#ifndef JSONSESSIONLOADER_H
#define JSONSESSIONLOADER_H

#include <QObject>

class JsonSessionLoader : public QObject
{
    Q_OBJECT
public:
    explicit JsonSessionLoader(QObject *t_Parent = nullptr);
    const QString &getSessionFilePath() const;

signals:
    /**
   * @brief Emitted for every module json object in a session file
   * @param t_uniqueModuleName
   * @param t_xmlPath Path to save back the settings if they were changed
   * @param t_xmlData
   * @param t_moduleId This is the module object entity id
   */
    void sigLoadModule(QString t_uniqueModuleName, QString t_xmlPath, QByteArray t_xmlData, int t_moduleId);

public slots:
    void loadSession(QString t_filePath);

private:
    void parseModule(QJsonObject moduleObject);
    QString m_currentSessionFile;
};

#endif // JSONSESSIONLOADER_H
