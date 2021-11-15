#ifndef CSOURCEJSONPARAMAPI_H
#define CSOURCEJSONPARAMAPI_H

#include <QJsonObject>

class cSourceJsonStructureApi
{
public:
    cSourceJsonStructureApi(QJsonObject paramStructure);

    int getCountUPhases();
    int getCountIPhases();
    QByteArray getIoPrefix();
private:
    QJsonObject m_paramStructure;
};

class cSourceJsonParamApi
{
public:
    cSourceJsonParamApi();

    QJsonObject getParams();
    void setParams(QJsonObject params);

    bool getOn();
    void setOn(bool on);

    double getRms(bool u, int phaseIdxBase0);
    double getAngle(bool u, int phaseIdxBase0);
    bool getOn(bool u, int phaseIdxBase0);

    bool getFreqVarOn();
    double getFreqVal();
private:
    QString getPhaseName(bool u, int phaseIdxBase0);

    QJsonObject m_params;
};

#endif // CSOURCEJSONPARAMAPI_H
