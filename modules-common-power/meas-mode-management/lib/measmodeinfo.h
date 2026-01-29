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
    m3ls,
    m3lsg,
    m2lw,
    m2lb,
    m2ls,
    m2lsg,
    mXlw,
    mXlb,
    mXls,
    mXlsg,
    mqref
};

enum sumtypes
{
    sum_phasevals = 0,
    sum_phase_pq_geom
};

constexpr int MeasModeFirst = m4lw;
constexpr int MeasModeCount = mqref+1;

class cMeasModeInfo
{
public:
    cMeasModeInfo(){}
    cMeasModeInfo(const QString &name,
                  const QString &actvalname,
                  const QString &unitname,
                  measmodes code);
    bool isValid() const;
    QString getName() const;
    QString getActvalName() const;
    QString getUnitName() const;
    measmodes getCode() const;
    sumtypes getSumTypeCode() const;
    bool isThreeWire() const;
private:
    QString m_sName;
    QString m_sActvalName;
    QString m_sUnitName;
    measmodes m_nCode;
};

#endif // MEASMODEINFO_H
