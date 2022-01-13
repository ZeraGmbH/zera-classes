#ifndef SOURCEJSONSWITCHERTEST_H
#define SOURCEJSONSWITCHERTEST_H

#include <QObject>

class SourceJsonSwitcherTest : public QObject
{
    Q_OBJECT

private slots:
    void init();

    void signalSwitch();
    void twoSignalsSwitchSameTwice();
    void busyToggledOnSwitch();

    void ioLastLongerThanLifetime();
};

#endif // SOURCEJSONSWITCHERTEST_H
