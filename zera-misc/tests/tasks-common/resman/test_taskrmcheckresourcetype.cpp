#include "test_taskrmcheckresourcetype.h"
#include "taskrmcheckresourcetype.h"
#include "rminterfacefortest.h"
#include "tasktesthelper.h"
#include <QTest>

QTEST_MAIN(test_taskrmcheckresourcetype)

void test_taskrmcheckresourcetype::okOnMatchingResourceLowerCase()
{
    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, "sense:foo");
    AbstractRmInterfacePtr rmInterface = RmInterfaceForTest::create(answers);

    TaskCompositePtr task = TaskRmCheckResourceType::create(rmInterface, DELAY_TIME);
    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskrmcheckresourcetype::okOnMatchingResourceUpperCase()
{
    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, "SENSE:FOO");
    AbstractRmInterfacePtr rmInterface = RmInterfaceForTest::create(answers);

    TaskCompositePtr task = TaskRmCheckResourceType::create(rmInterface, DELAY_TIME);
    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskrmcheckresourcetype::errorOnNoResources()
{
    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, "");
    AbstractRmInterfacePtr rmInterface = RmInterfaceForTest::create(answers);

    TaskCompositePtr task = TaskRmCheckResourceType::create(rmInterface, DELAY_TIME);
    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
}

void test_taskrmcheckresourcetype::errorOnMissingResource()
{
    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, "foo:bar");
    AbstractRmInterfacePtr rmInterface = RmInterfaceForTest::create(answers);

    TaskCompositePtr task = TaskRmCheckResourceType::create(rmInterface, DELAY_TIME);
    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
}
