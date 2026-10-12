#ifndef TEST_SECPOWERMODULEOBSERVER_H
#define TEST_SECPOWERMODULEOBSERVER_H

#include "modulemanagertestrunner.h"
#include <QObject>

class test_secpowermoduleobserver : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();

    void signalOnEntityIdInitialSet();
    void signalOnEntityIdInitialSetTwiceSame();
    void entityGetOnSignal();
    void changeEntity();

    void initialPowerType();
    void getPowertypePower1();
    void getPowertypePower2();
    void getPowertypeInvalid();

    void initialConstant();
    void getConstantPower1();
    void getConstantPower2();
    void getConstantPowerInvalid();

    void changeMeasMode();
    void changeEntityIdThenMeasMode();

private:
    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
};

#endif // TEST_SECPOWERMODULEOBSERVER_H
