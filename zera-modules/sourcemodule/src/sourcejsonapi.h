#ifndef CSOURCEJSONPARAMAPI_H
#define CSOURCEJSONPARAMAPI_H

#include <QJsonObject>

class SourceJsonStructApi
{
public:
    SourceJsonStructApi(QJsonObject paramStructure);

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


enum class phaseType{
    U, //Voltage
    I  //Current
};

class SourceJsonParamApi
{
public:
    SourceJsonParamApi();

    QJsonObject getParams() const;
    void setParams(QJsonObject params);

    bool getOn() const;
    void setOn(bool on);

    double getRms(phaseType type, int phaseIdxBase0) const;
    double getAngle(phaseType type, int phaseIdxBase0) const;
    bool getOn(phaseType type, int phaseIdxBase0) const;

    bool getFreqVarOn() const;
    double getFreqVal() const;
private:
    QString getPhaseName(phaseType type, int phaseIdxBase0) const;
    QString getPhaseNamePrefix(phaseType type) const;

    QJsonObject m_params;
};

#endif // CSOURCEJSONPARAMAPI_H
