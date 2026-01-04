#ifndef SOURCESWITCHJSONTEST_H
#define SOURCESWITCHJSONTEST_H

#include <QObject>

class test_sourceswitchjson : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void signalSwitch();
    void signalSwitchAfterError();
    void twoSignalsSwitchSameTwice();

    void currentAndRequestedParamOnError();
    void changeParamOnSuccess();
};

#endif // SOURCESWITCHJSONTEST_H
