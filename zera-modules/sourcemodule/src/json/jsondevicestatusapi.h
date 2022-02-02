#ifndef JSONDEVICESTATUS_H
#define JSONDEVICESTATUS_H

#include <QJsonObject>

class JsonDeviceStatusApi
{
public:
    JsonDeviceStatusApi();

    void reset();
    void clearWarningsErrors();

    void setBusy(bool busy);
    void addError(const QString error);
    void addWarning(const QString warning);
    void setDeviceInfo(const QString strDeviceInfo);

    const QJsonObject &getJsonStatus();
    bool getBusy() const;
    QStringList getErrors() const;
    QStringList getWarnings() const;
    QString getDeviceInfo() const;
private:
    QStringList getArray(QString key) const;
    void appendToArray(QString key, QString value);

    QJsonObject m_jsonStatus;
};

#endif // JSONDEVICESTATUS_H
