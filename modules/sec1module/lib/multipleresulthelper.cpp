#include "multipleresulthelper.h"
#include <QJsonObject>
#include <QDateTime>
#include <math.h>

void MultipleResultHelper::clear()
{
    m_fAccumulSum = 0.0;
    m_fMeanValue = qQNaN();
    m_fStdDevn = qQNaN();
    m_fStdDevn1 = qQNaN();
    m_jsonResultArray = QJsonArray();
    m_iCountPass = 0;
    m_iCountUnfinish = 0;
    m_iCountFail = 0;
    m_iIdxMinValue = -1;
    m_iIdxMaxValue = -1;
    m_fMinError = qQNaN();
    m_fMaxError = qQNaN();
    m_jsonLimitArray = QJsonArray();
    m_fLastLowerLimit = qQNaN();
    m_fLastUpperLimit = qQNaN();
}

void MultipleResultHelper::append(const double fResult,
                                  const ECALCRESULT::enResultTypes eRating,
                                  const double fLowerLimit,
                                  const double fUpperLimit,
                                  const double fMaxError,
                                  const QString startTime,
                                  const QString endTime)
{
    // limits
    // note: limits are not results of a calculation so we can comape on equality
    if(m_fLastLowerLimit != fLowerLimit || m_fLastUpperLimit != fUpperLimit) {
        QJsonObject jsonObjectLimits {
            { "IDX", m_jsonResultArray.count() /* value not added yet */ },
            { "LL", fLowerLimit },
            { "UL", fUpperLimit }
        };
        m_jsonLimitArray.append(jsonObjectLimits);
        m_fLastLowerLimit = fLowerLimit;
        m_fLastUpperLimit = fUpperLimit;
    }
    QJsonObject jsonObjectValue {
        { "V", fResult },
        { "R", int(eRating) },
        { "S", startTime },
        { "T", endTime }
    };
    m_jsonResultArray.append(jsonObjectValue);
    // some statistics
    switch(eRating) {
    case ECALCRESULT::RESULT_UNFINISHED:
        ++m_iCountUnfinish;
        break;
    case ECALCRESULT::RESULT_PASSED:
        ++m_iCountPass;
        break;
    default:
        ++m_iCountFail;
        break;
    }
    double value = fResult;
    // use +/- 100% for inf/nan/unfinished in statistics
    if(qIsNaN(value)) {
        value = -fMaxError;
    }
    else if(qIsInf(value)) {
        value = fMaxError;
    }
    if(eRating == ECALCRESULT::RESULT_UNFINISHED) { // yes have seen that on HK
        value = -fMaxError;
    }
    // value has been added above
    int iResultCount = m_jsonResultArray.count();
    // range
    if(value < m_fMinError || iResultCount == 1) {
        m_fMinError = value;
        m_iIdxMinValue = iResultCount-1;
    }
    if(value > m_fMaxError || iResultCount == 1) {
        m_fMaxError = value;
        m_iIdxMaxValue = iResultCount-1;
    }
    // mean value
    m_fAccumulSum += value;
    double fResultCount = iResultCount;
    m_fMeanValue = m_fAccumulSum / fResultCount;

    // standard deviations: There have been loads of discussons about the n or
    // n-1 in the past. To avoid that: calc both
    double quadratDevSum = 0.0;
    for(int idx=0; idx<iResultCount; ++idx) {
        double currValue = m_jsonResultArray[idx].toObject()["V"].toDouble();
        double currDeviation = currValue - m_fMeanValue;
        quadratDevSum += currDeviation * currDeviation;
    }
    m_fStdDevn = sqrt(quadratDevSum / fResultCount);
    if(iResultCount > 1) {
        m_fStdDevn1 = sqrt(quadratDevSum / (fResultCount - 1.0));
    }
    else {
        m_fStdDevn1 = qQNaN();
    }
}

const QJsonArray &MultipleResultHelper::getJSONResultArray()
{
    return m_jsonResultArray;
}

const QJsonArray &MultipleResultHelper::getJSONLimitsArray()
{
    return m_jsonLimitArray;
}

const QJsonObject MultipleResultHelper::getJSONStatistics()
{
    QJsonObject jsonObject {
        {QStringLiteral("countPass"), m_iCountPass},
        {QStringLiteral("countFail"), m_iCountFail},
        {QStringLiteral("countUnfinish"), m_iCountUnfinish},
        {QStringLiteral("mean"), m_fMeanValue},
        {QStringLiteral("range"), getCountTotal() > 0 ? m_fMaxError-m_fMinError : qQNaN()},
        {QStringLiteral("idxMin"), m_iIdxMinValue},
        {QStringLiteral("idxMax"), m_iIdxMaxValue},
        {QStringLiteral("stddevN"), m_fStdDevn},
        {QStringLiteral("stddevN1"), m_fStdDevn1}
    };
    return jsonObject;
}

quint32 MultipleResultHelper::getCountTotal()
{
    return m_jsonResultArray.count();
}
