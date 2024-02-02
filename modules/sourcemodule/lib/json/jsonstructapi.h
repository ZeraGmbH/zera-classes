#ifndef JSONSTRUCTAPI_H
#define JSONSTRUCTAPI_H

#include <QJsonObject>

class JsonStructApi
{
public:
    JsonStructApi(QJsonObject paramStructure);

    QJsonObject getParamStructure() const;

    QString getDeviceName() const;
    QString getDeviceVersion() const;
    int getCountUPhases() const;
    int getCountIPhases() const;
    QByteArray getIoPrefix() const;

    void setDeviceName(QString name);
    void setDeviceVersion(QString version);
private:
    QJsonObject m_paramStructure;
};

#endif // JSONSTRUCTAPI_H
