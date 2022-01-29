#ifndef SOURCESCANNERTEST_H
#define SOURCESCANNERTEST_H

#include "source-scanner/sourcescanner.h"
#include <QObject>

class SourceScannerTest : public QObject
{
    Q_OBJECT

public slots:
    void onScanFinishedForCheckInstanceCount(SourceScanner::Ptr scanner);
private slots:
    void init();
    void scannerDiesOnNoConnection();
    void scannerSurvivesUntilSlotDirectConnection();
    void scannerSurvivesUntilSlotQueuedConnection();
    void scannerSurvivesUntilSlotLambdaConnection();

    void scannerReportsInvalidSourceAfterCreation();
    void scannerReportsValidSourceAfterDemoIo();
    void scannerReportsValidSourceAfterZeraIo();
    void scannerReportsInvalidSourceAfterBrokenIo();
    void scannerReportsInvalidSourceAfterDemoIoResponseError();
    void scannerReportsInvalidSourceAfterZeraIoResponseError();

    void uuidPassedIdentical();
    void ioDevicePassedIdentical();

private:
    int m_scanFinishCount;
};

#endif // SOURCESCANNERTEST_H
