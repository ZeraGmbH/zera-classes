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

    void singleDemoResponseLeadTrail();
    void singleDemoResponseLeadTrailEmpty();
    void singleDemoResponseSimError();

    void singleDataEvalNotExecuted();
    void singleDataEvalNoAnswer();
    void singleDataEvalWrongAnswer();
    void singleDataEvalPass();

    void singleCheckUnusedDataOnOnConstruct1();
    void singleCheckUnusedDataOnOnConstruct2();
    void singleCheckUsedDataDataReceived();
    void singleCheckUsedDataNoAnswer();
    void singleCheckUsedWrongAnswer();
    void singleCheckUsedPass();

    void groupEmptyInvalid();
    void groupIdsCreated();

    void groupEqual();
    void groupUnequal();
    void groupSignalSlotDirect();
    void groupSignalSlotQueues();

    void groupIsNotSwitchGroup();
    void groupIsSwitchGroup();
    void groupIsQueryGroup();
    void groupIsNotQueryGroup();
    void groupIsUnknownGroup();
    void groupTypeHasIsMethod();

    /*
    void groupPassedAllFail();
    void groupPassedAllOk();
    */
};

#endif // IOTRANSFERDATATEST_H
