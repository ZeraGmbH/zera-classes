#ifndef MEASMODEINFO_H
#define MEASMODEINFO_H

#include <QString>

namespace POWER1MODULE
{

enum powermodes
{
    actPower, reactPower, appPower
};


class cMeasModeInfo
{
public:
    cMeasModeInfo(){}
    cMeasModeInfo(QString name, QString actvalname, QString unitname,int powmode, int code);
    QString getName();
    QString getActvalName();
    QString getUnitName();
    quint8 getPowerMode();
    quint8 getCode();

private:
    QString m_sName;
    QString m_sActvalName;
    QString m_sUnitName;
    quint8 m_nPowerMode; // actPower, reactPower, appPower
    quint8 m_nCode;
};

}

#endif // MEASMODEINFO_H
