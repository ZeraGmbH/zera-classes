#ifndef SOURCEJSONSWITCHERTEST_H
#define SOURCEJSONSWITCHERTEST_H

#include <QObject>

class SourceDeviceSwitcherJsonTest : public QObject
{
    Q_OBJECT

private slots:
    void init();

    void signalSwitch();
    void twoSignalsSwitchSameTwice();
};

#endif // SOURCEJSONSWITCHERTEST_H
