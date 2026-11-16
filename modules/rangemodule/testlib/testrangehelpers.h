#ifndef TESTRANGEHELPERS_H
#define TESTRANGEHELPERS_H

#include <QString>
#include <QSignalSpy>

class TestRangeHelpers
{
public:
    enum CorrectionType
    {
        OFFSET,
        GAIN,
        PHASE
    };
    static void setCorrectionCoefficient(const QString &channelMName, const QString &rangeName,
                                          CorrectionType type, int order, double value);
    static void initCoefficients(const QString &channelMName, const QString &rangeName);
    static float findDspWriteValue(const QSignalSpy &spyDspVarWrite, const QString &dspVarName, int dspChannelNr);

private:
    static QString stringifyType(CorrectionType type);
};

#endif // TESTRANGEHELPERS_H
