#ifndef IOTRANSFERDATATEST_H
#define IOTRANSFERDATATEST_H

#include <QObject>

class IoTransferDataTest : public QObject
{
    Q_OBJECT
private slots:
    void init();

    void singleDataEvalNotExecutedOnConstruct1();
    void singleDataEvalNotExecutedOnConstruct2();
    void singleDataEvalNotExecutedOnConstruct3();

    void singleDemoResponseLeadTrail();
    void singleDemoResponseLeadTrailEmpty();
    void singleDemoResponseSimError();

    void singleDataEvalNotExecuted();
    void singleDataEvalNoAnswer();
    void singleDataEvalWrongAnswerConstructor1();
    void singleDataEvalWrongAnswerConstructor2();
    void singleDataEvalWrongAnswerConstructor3();
    void singleDataEvalPassConstructor1();
    void singleDataEvalPassConstructor2();
    void singleDataEvalPassConstructor3();

    void singleCheckUnusedDataOnOnConstruct1();
    void singleCheckUnusedDataOnOnConstruct2();
    void singleCheckUnusedDataOnOnConstruct3();
    void singleCheckUsedDataDataReceived();
    void singleCheckUsedDataNoAnswer();
    void singleCheckUsedWrongAnswer();
    void singleCheckUsedPass();

    void groupIdsCreated();

    void groupPassedAllFail();
};

#endif // IOTRANSFERDATATEST_H
