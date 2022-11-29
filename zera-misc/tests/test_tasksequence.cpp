#include "test_tasksequence.h"
#include "tasksequence.h"
#include "taskfortest.h"
#include <QTest>
#include <QElapsedTimer>

void test_tasksequence::oneOk()
{
    TaskSequencePtr taskSeq = TaskSequence::create();
    taskSeq->addTask(TaskForTest::create(0, false));
    int okCount = 0;
    connect(taskSeq.get(), &TaskSequence::finishOk, [&]() {
        okCount++;
    });
    taskSeq->start();
    QCoreApplication::processEvents();
    QCOMPARE(okCount, 1);
}

QTEST_MAIN(test_tasksequence)

