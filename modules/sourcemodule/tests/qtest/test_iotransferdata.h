#ifndef IOTRANSFERDATATEST_H
#define IOTRANSFERDATATEST_H

#include <QObject>

class test_iotransferdata : public QObject
{
    Q_OBJECT
private slots:
    void init();

    void singleDataEvalNotExecutedOnConstruct1();
    void singleDataEvalNotExecutedOnConstruct2();

    void singleDemoResponseLeadTrail();
    void singleDemoResponseLeadTrailEmpty();
    void singleDemoResponseSimError();

    void singleQueryContent();
    void singleQueryContentPass();
    void singleQueryContentFail();
    void singleCustomQueryEvaluatorNotSet();
    void singleCustomQueryEvaluatorSet();

    void singleDataEvalNotExecuted();
    void singleDataEvalNoAnswer();
    void singleDataEvalWrongAnswerConstruct1();
    void singleDataEvalWrongAnswerConstruct2();
    void singleDataEvalPassConstruct1();
    void singleDataEvalPassConstruct2();

    void singleCheckUnusedDataOnOnConstruct1();
    void singleCheckUnusedDataOnOnConstruct2();
    void singleCheckUsedDataDataReceived();
    void singleCheckUsedDataNoAnswer();
    void singleCheckUsedWrongAnswer();
    void singleCheckUsedPass();

    void singleCheckInjectSingleExpected();
    void singleCheckInjectMultipleExpected();

    void groupIdsCreated();

    void groupPassedAllFail();
};

#endif // IOTRANSFERDATATEST_H
