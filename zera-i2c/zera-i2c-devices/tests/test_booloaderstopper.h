#ifndef TEST_BOOLOADERSTOPPER_H
#define TEST_BOOLOADERSTOPPER_H

#include <QObject>

class test_booloaderstopper : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void applicationRunning();
    void bootloaderRunning();
};

#endif // TEST_BOOLOADERSTOPPER_H
