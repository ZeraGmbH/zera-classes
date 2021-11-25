#ifndef CSOURCEDEVICESTATUS_H
#define CSOURCEDEVICESTATUS_H

#include <QJsonObject>

namespace SOURCEMODULE
{

class cSourceDeviceStatus
{
public:
    cSourceDeviceStatus();

    void reset();

    void setBusy(bool busy);
    void addError(const QString error);
    void addWarning(const QString warning);

    const QJsonObject &getJsonStatus();
    bool getBusy();
    QStringList getErrors();
    QStringList getWarnings();
private:
    QStringList getArray(QString key);
    void appendToArray(QString key, QString value);

    QJsonObject m_jsonStatus;
};

} //namespace SOURCEMODULE

#endif // CSOURCEDEVICESTATUS_H
