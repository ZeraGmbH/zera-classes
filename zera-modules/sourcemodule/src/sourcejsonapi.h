#ifndef CSOURCEJSONPARAMAPI_H
#define CSOURCEJSONPARAMAPI_H

#include <QJsonObject>

class cSourceJsonStructApi
{
public:
    cSourceJsonStructApi(QJsonObject paramStructure);

    int getCountUPhases();
    int getCountIPhases();
    QByteArray getIoPrefix();
private:
    QJsonObject m_paramStructure;
};


enum class phaseType{
    U, //Voltage
    I  //Current
};

class cSourceJsonParamApi
{
public:
    cSourceJsonParamApi();

    QJsonObject getParams();
    void setParams(QJsonObject params);

    bool getOn();
    void setOn(bool on);

    double getRms(phaseType type, int phaseIdxBase0);
    double getAngle(phaseType type, int phaseIdxBase0);
    bool getOn(phaseType type, int phaseIdxBase0);

    bool getFreqVarOn();
    double getFreqVal();
private:
    QString getPhaseName(phaseType type, int phaseIdxBase0);
    QString getPhaseNamePrefix(phaseType type);

    QJsonObject m_params;
};

#endif // CSOURCEJSONPARAMAPI_H
