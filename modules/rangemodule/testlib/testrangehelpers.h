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
    static float testPolynom(double x, double coeff0, double coeff1 = 0.0, double coeff2 = 0.0, double coeff3 = 0.0);

private:
    static QString stringifyType(CorrectionType type);
};

#endif // TESTRANGEHELPERS_H
