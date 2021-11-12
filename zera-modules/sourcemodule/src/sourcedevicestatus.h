#ifndef CSOURCEDEVICESTATUS_H
#define CSOURCEDEVICESTATUS_H

#include <QJsonObject>

namespace SOURCEMODULE
{

class cSourceDeviceStatus
{
public:
    cSourceDeviceStatus();

    const QJsonObject &getJsonStatus();
    void reset();

    void setBusy(bool busy);
    void addError(const QString error);
    void addWarning(const QString warning);

private:
    QJsonObject m_jsonStatus;
};

} //namespace SOURCEMODULE

#endif // CSOURCEDEVICESTATUS_H
