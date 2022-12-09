#ifndef TEST_TASKEXTRAERRORHANDLER_H
#define TEST_TASKEXTRAERRORHANDLER_H

#include <QObject>

class test_taskextraerrorhandler : public QObject
{
    Q_OBJECT
private slots:
    void handleError();
    void handleNoError();
};

#endif // TEST_TASKEXTRAERRORHANDLER_H
