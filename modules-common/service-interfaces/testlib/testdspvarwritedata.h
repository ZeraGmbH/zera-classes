#ifndef TESTDSPVARWRITEDATA_H
#define TESTDSPVARWRITEDATA_H

#include <QObject>
#include <QString>
#include <QVector>

struct TestDspVarWriteData
{
    QString varName;
    QVector<float> values;

    int transactionCount = 0;
    int currentTimeMs = 0;
};

Q_DECLARE_METATYPE(TestDspVarWriteData)

#endif // TESTDSPVARWRITEDATA_H
