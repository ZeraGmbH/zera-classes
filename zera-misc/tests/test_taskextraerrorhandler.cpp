#include "test_taskextraerrorhandler.h"
#include "taskextraerrorhandler.h"
#include "taskfortest.h"
#include <QTest>

QTEST_MAIN(test_taskextraerrorhandler)

void test_taskextraerrorhandler::handleError()
{
    int errorCount = 0;
    TaskCompositePtr task = TaskExtraErrorHandler::create(TaskForTest::create(0, false),[&]{
        errorCount++;
    });

    task->start();
    QCOMPARE(errorCount, 1);
}

void test_taskextraerrorhandler::handleNoError()
{
    int errorCount = 0;
    TaskCompositePtr task = TaskExtraErrorHandler::create(TaskForTest::create(0, true),[&]{
        errorCount++;
    });

    task->start();
    QCOMPARE(errorCount, 0);
}

