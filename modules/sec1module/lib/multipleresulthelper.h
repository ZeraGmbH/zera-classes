#ifndef MULTIPLERESULTHELPER_H
#define MULTIPLERESULTHELPER_H

#include <secinterface.h>
#include <QJsonArray>


/**
 * @brief Class MultipleResultHelper: Collect multple results / calculate statistics / out as JSON
 */
class MultipleResultHelper {
public:
    /**
     * @brief clear: Clear resulr array / reset statistics
     */
    void clear();
    void append(const double fResult,
                const enum ECALCRESULT::enResultTypes eRating,
                const double fLowerLimit,
                const double fUpperLimit,
                const double fMaxError,
                const QString startTime, const QString endTime);
    /**
     * @brief getCountTotal
     * @return Number of results in array
     */
    quint32 getCountTotal();
    /**
     * @brief getJSONResultArray: return JSON result array
     * @note field names are as short as possible to reduce size
     * @return JSON array as: { "V" : ResultValue,
     *                          "R" : <Rating(Evaluation)>
     *                          "T" : <Date/Time of result in short format 'yyyyMMddhhmmss'>
     *                        }
     */
    const QJsonArray &getJSONResultArray();
    /**
     * @brief getJSONResultLimits: return JSON result limit array
     * @note In most cases (constant limits during measurement) limit array
     * has one entry only
     * @return JSON array as: { "IDX": <start-index in results>
     *                          "LL": <LowerLimit>,
     *                          "UL": <UpperLimit>,
     *                        }
     */
    const QJsonArray &getJSONLimitsArray();
    /**
     * @brief getJSONStatistics: return JSON object containing simple statistic values
     * @return { "countPass: <count of passed results>,
     *           "countFail": <count of passed results>,
     *           "countUnfinish": <count of unfinished results>,
     *           "mean": <mean (average) value,
     *           "range": <maxError - minError>
     *           "idxMin": <index of min error>
     *           "idxMax": <index of max error>
     *           "stddevN": <statndard deviantion / n in denominator,
     *           "stddevN1": <statndard deviantion / (n-1) in denominator,
     *          }
     */
    const QJsonObject getJSONStatistics();
private:
    // mean / stddev
    double m_fMeanValue = qQNaN();
    double m_fStdDevn = qQNaN();
    double m_fStdDevn1 = qQNaN();
    // range
    int m_iIdxMinValue = -1;
    int m_iIdxMaxValue = -1;
    double m_fMinError = qQNaN();
    double m_fMaxError = qQNaN();
    // results / counters
    QJsonArray m_jsonResultArray;
    int m_iCountPass = 0;
    int m_iCountFail = 0;
    int m_iCountUnfinish = 0;
    // error estimation limits
    QJsonArray m_jsonLimitArray;
    double m_fLastLowerLimit = qQNaN();
    double m_fLastUpperLimit = qQNaN();
    /**
     * @note To avoid one iteration of all results we keep accumulated sum
     */
    double m_fAccumulSum = 0.0;
};

#endif // MULTIPLERESULTHELPER_H
