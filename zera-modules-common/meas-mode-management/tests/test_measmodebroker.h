#ifndef TEST_MEASMODEBROKER_H
#define TEST_MEASMODEBROKER_H

#include <QObject>

class test_measmodebroker : public QObject
{
    Q_OBJECT
private slots:
    void getOneCheckDspCmd();
    void getOneCheckSelectionInDspCmd();
    void getTwoCheckDspCmd();
    void getTwoModesWithSameDspCmdSecondCmdEmpty();
    void getTwoModesWithSameCheckAllReturnsSet();
    void getTwoCheckDspSelectionSame();
    void singleCheckDspCmdIsValid();
    void getFooIsInvalid();
    void getTwoWithDiffDspSelection();
    void getTwoCheckPhaseStartegies();
};

#endif // TEST_MEASMODEBROKER_H
