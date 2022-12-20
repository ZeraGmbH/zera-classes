#include "test_taskservertransactiontemplate.h"
#include "taskservertransactiontemplatetest.h"
#include "rminterfacefortest.h"
#include "taskfortest.h"
#include "tasktesthelper.h"
#include <QTest>

QTEST_MAIN(test_taskservertransactiontemplate)

static const char* defaultResponse = "m0;m1;m2";

void test_taskservertransactiontemplate::actionOnAck()
{
    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, defaultResponse);
    AbstractRmInterfacePtr rmInterface = RmInterfaceForTest::create(answers);

    QString received;
    TaskCompositePtr task = TaskServerTransactionTemplateTest::create(rmInterface, received);
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(received, defaultResponse);
}

void test_taskservertransactiontemplate::noActionOnNack()
{
    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(nack, defaultResponse);
    AbstractRmInterfacePtr rmInterface = RmInterfaceForTest::create(answers);

    QString received;
    TaskCompositePtr task = TaskServerTransactionTemplateTest::create(rmInterface, received);
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(received, "");
}

void test_taskservertransactiontemplate::okSignalOnAck()
{
    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, defaultResponse);
    AbstractRmInterfacePtr rmInterface = RmInterfaceForTest::create(answers);

    QString received;
    TaskCompositePtr task = TaskServerTransactionTemplateTest::create(rmInterface, received);

    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskservertransactiontemplate::errSignalOnNack()
{
    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(nack, defaultResponse);
    AbstractRmInterfacePtr rmInterface = RmInterfaceForTest::create(answers);

    QString received;
    TaskCompositePtr task = TaskServerTransactionTemplateTest::create(rmInterface, received);

    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
}

void test_taskservertransactiontemplate::noReceiveOnOther()
{
    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, defaultResponse, RmTestAnswer::MSG_ID_OTHER);
    AbstractRmInterfacePtr rmInterface = RmInterfaceForTest::create(answers);

    QString received;
    TaskCompositePtr task = TaskServerTransactionTemplateTest::create(rmInterface, received);

    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskservertransactiontemplate::errReceiveOnTcpError()
{
    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, defaultResponse, RmTestAnswer::TCP_ERROR);
    AbstractRmInterfacePtr rmInterface = RmInterfaceForTest::create(answers);

    QString received;
    TaskCompositePtr task = TaskServerTransactionTemplateTest::create(rmInterface, received);

    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);

}


