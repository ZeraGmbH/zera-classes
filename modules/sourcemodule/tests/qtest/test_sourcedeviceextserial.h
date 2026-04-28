#ifndef TEST_SOURCEDEVICEEXTSERIAL_H
#define TEST_SOURCEDEVICEEXTSERIAL_H

#include <QObject>

class test_sourcedeviceextserial : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void checkDeviceInfo();
    void checkDemoOn();
    void checkDemoOff();

    void checkVeinInitialStatus();
    void checkVeinInitialInfo();
    void checkVeinInitialLoad();

    void checkVeinSwitchTwoStateChanges();
    void checkVeinSwitchChangesLoad();

    void checkVeinSwitchError();
    void checkVeinStateError();
};

#endif // TEST_SOURCEDEVICEEXTSERIAL_H
