#include "testrangehelpers.h"
#include <testdspvarwritedata.h>
#include <scpisingletransactionblocked.h>
#include <zscpi_response_definitions.h>
#include <QTest>

void TestRangeHelpers::setCorrectionCoefficient(const QString &channelMName, const QString &rangeName,
                                                         CorrectionType type, int order, double value)
{
    QString scpiCmd = QString("SENS:%1:%2:CORR:%3:COEFFICIENT:%4")
                          .arg(channelMName, rangeName, stringifyType(type))
                          .arg(order);
    QString answer = ScpiSingleTransactionBlocked::cmd(scpiCmd, QString::number(value, 'g', 12));
    QCOMPARE(answer, ZSCPI::scpiAnswer[ZSCPI::ack]);
}

void TestRangeHelpers::initCoefficients(const QString &channelMName, const QString &rangeName)
{
    QString scpiCmd = QString("SENS:%1:%2:CORR:INIT").arg(channelMName, rangeName);
    QString answer = ScpiSingleTransactionBlocked::cmd(scpiCmd, "");
    QCOMPARE(answer, ZSCPI::scpiAnswer[ZSCPI::ack]);
}

float TestRangeHelpers::findDspWriteValue(const QSignalSpy &spyDspVarWrite, const QString &dspVarName, int dspChannelNr)
{
    for (int i = 0; i < spyDspVarWrite.count(); ++i) {
        TestDspVarWriteData writtenVarData = spyDspVarWrite.at(i).at(0).value<TestDspVarWriteData>();
        if (writtenVarData.varName == dspVarName)
            return writtenVarData.values[dspChannelNr];
    }
    qCritical("No DSP write for group: %s found", qPrintable(dspVarName));
    return 0.0f;
}

// see AdjDataItem::getCorrection
float TestRangeHelpers::testPolynom(double x, double coeff0, double coeff1, double coeff2, double coeff3)
{
    double result = coeff0; // getCorrection starts with Corr += c0 * 1.0
    double arg = x;
    result += coeff1 * arg;
    arg *= x;
    result += coeff2 * arg;
    arg *= x;
    result += coeff3 * arg;
    return float(QString::number(result, 'g', 6).toDouble()); // MT310s2 SCPI answer quantization (why exactly??)
}

QString TestRangeHelpers::stringifyType(CorrectionType type)
{
    switch (type) {
    case OFFSET:
        return "OFFSET";
    case GAIN:
        return "GAIN";
    case PHASE:
        return "PHASE";
    }
    return "";
}
