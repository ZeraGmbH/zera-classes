#ifndef TEST_BOOTLOADERSTOPPERFACTORY_H
#define TEST_BOOTLOADERSTOPPERFACTORY_H

#include <QObject>

class test_bootloaderstopperfactoryfortest : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void checkEmpty();
    void oneImmediate();
    void oneDelayed();
    void oneDelayedOneImmediate();
};

#endif // TEST_BOOTLOADERSTOPPERFACTORY_H
