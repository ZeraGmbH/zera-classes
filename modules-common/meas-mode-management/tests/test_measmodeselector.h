#ifndef TEST_MEASMODESELECTOR_H
#define TEST_MEASMODESELECTOR_H

#include <QObject>

class test_measmodeselector : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void initCurrentModeInvalid();
    void initCannotChangeMask();
    void initInactiveMask();
    void addCannotChangeMask();
    void invalidNotAdded();
    void doubleNotAdded();
    void addAndSelectValidMode();
    void addValidSelectInvalidMode();
    void addTwoAndSelectThem();
    void addAndSelectAndChangeMask();
    void addAndSelectAndChangeInvalidMask();
    void addTwoSelectThemChangePhases();
};

#endif // TEST_MEASMODESELECTOR_H
