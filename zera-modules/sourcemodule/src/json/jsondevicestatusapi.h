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
    bool getBusy() const;
    void addError(const QString error);
    QStringList getErrors() const;
    void addWarning(const QString warning);
    QStringList getWarnings() const;
    void setDeviceInfo(const QString strDeviceInfo);
    QString getDeviceInfo() const;
    const QJsonObject &getJsonStatus();

private:
    QStringList getArray(QString key) const;
    void appendToArray(QString key, QString value);
    QJsonObject m_jsonStatus;
};

#endif // JSONDEVICESTATUS_H
