#ifndef SECCONFIGDATACOMMON_H
#define SECCONFIGDATACOMMON_H

#include "configtypes.h"
#include <QString>
#include <QList>

struct TRefInput
{
    QString inputName;
    QString alias;
};

struct TSecCommonReferenceConfigs
{
    quint8 m_nRefInpCount = 0;
    QList<TRefInput> m_refInpList;
    stringParameter m_sRefInput;
};

struct TSecCommonDutConfigs
{
    quint8 m_nDutInpCount = 0;
    QList<QString> m_dutInpList;
    stringParameter m_sDutInput;
};

struct TSecCommonLimitConfigs
{
    doubleParameter m_fUpperLimit;
    doubleParameter m_fLowerLimit;
};

struct TSecCommonUnitConfigs
{
    quint8 m_nActiveUnitCount = 0;
    QList<QString> m_ActiveUnitList;
    quint8 m_nReactiveUnitCount = 0;
    QList<QString> m_ReactiveUnitList;
    quint8 m_nApparentUnitCount = 0;
    QList<QString> m_ApparentUnitList;
};

#endif // SECCONFIGDATACOMMON_H
