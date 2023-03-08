#ifndef MEASMODEINFO_H
#define MEASMODEINFO_H

#include <QString>

constexpr int MeasPhaseCount = 3;
constexpr int SumValueCount = 1;

enum measmodes
{
    m4lw,
    m4lb,
    m4lbk,
    m4ls,
    m4lsg,
    m3lw,
    m3lb,
    m2lw,
    m2lb,
    m2ls,
    m2lsg,
    mXlw,
    mqref
};

class cMeasModeInfo
{
public:
    cMeasModeInfo(){}
    cMeasModeInfo(QString name, QString actvalname, QString unitname, measmodes code);
    bool isValid() const;
    QString getName() const;
    QString getActvalName() const;
    QString getUnitName() const;
    measmodes getCode() const;
private:
    QString m_sName;
    QString m_sActvalName;
    QString m_sUnitName;
    measmodes m_nCode;
};

#endif // MEASMODEINFO_H
