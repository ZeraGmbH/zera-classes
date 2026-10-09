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

#endif // SECCONFIGDATACOMMON_H
