#ifndef CSOURCEDEVICESTATUS_H
#define CSOURCEDEVICESTATUS_H

#include <QJsonObject>

namespace SOURCEMODULE
{

class SourceDeviceStatus
{
public:
    SourceDeviceStatus();

    void reset();
    void clearWarningsErrors();

    void setBusy(bool busy);
    void addError(const QString error);
    void addWarning(const QString warning);
    void setDeviceInfo(const QString strDeviceInfo);

    const QJsonObject &getJsonStatus();
    bool getBusy();
    QStringList getErrors();
    QStringList getWarnings();
    QString getDeviceInfo();
private:
    QStringList getArray(QString key);
    void appendToArray(QString key, QString value);

    QJsonObject m_jsonStatus;
};

} //namespace SOURCEMODULE

#endif // CSOURCEDEVICESTATUS_H
