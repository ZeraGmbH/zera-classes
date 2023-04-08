#ifndef SOURCEDEVICEFACADETEST_H
#define SOURCEDEVICEFACADETEST_H

#include <QObject>

class test_sourcedevicefacade : public QObject
{
    Q_OBJECT
private slots:
    void init();

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

#endif // SOURCEDEVICEFACADETEST_H
