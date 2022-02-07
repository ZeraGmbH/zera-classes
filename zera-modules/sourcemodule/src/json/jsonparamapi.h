#ifndef JSONPARAMAPI_H
#define JSONPARAMAPI_H

#include <QJsonObject>

enum class phaseType{
    U,
    I
};

class JsonParamApi
{
public:
    JsonParamApi();
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

#endif // JSONPARAMAPI_H
