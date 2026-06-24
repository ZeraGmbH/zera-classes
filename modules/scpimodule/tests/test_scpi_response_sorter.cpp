#include "test_scpi_response_sorter.h"
#include "scpicmdresponsesorter.h"
#include <QTest>

QTEST_MAIN(test_scpi_response_sorter)

void test_scpi_response_sorter::transactionsAreValid()
{
    ScpiCmdResponseSorter sorter;

    QVERIFY(sorter.createTransaction().isValid());
    QVERIFY(sorter.createTransaction().isValid());
    QVERIFY(sorter.createTransaction().isValid());
}

void test_scpi_response_sorter::transactionsChrono()
{
    ScpiCmdResponseSorter sorter;
    ScpiTransactionId id1 = sorter.createTransaction();
    ScpiTransactionId id2 = sorter.createTransaction();
    ScpiTransactionId id3 = sorter.createTransaction();

    QVERIFY(id1.getChrono() < id2.getChrono());
    QVERIFY(id2.getChrono() < id3.getChrono());
}

void test_scpi_response_sorter::start_1_Response_1()
{
    ScpiCmdResponseSorter sorter;
    ScpiTransactionId transaction = sorter.createTransaction();

    QString response = "1";
    QStringList sortedResponses = sorter.genOrDelaySortedOutput(response, transaction);

    QCOMPARE(sortedResponses.size(), 1);
    QCOMPARE(sortedResponses[0], response);
}

void test_scpi_response_sorter::start_1_2_Response_1_2()
{
    ScpiCmdResponseSorter sorter;
    ScpiTransactionId transaction1 = sorter.createTransaction();
    ScpiTransactionId transaction2 = sorter.createTransaction();

    QString response1 = "1";
    QString response2 = "2";
    QStringList sortedResponses1 = sorter.genOrDelaySortedOutput(response1, transaction1);
    QStringList sortedResponses2 = sorter.genOrDelaySortedOutput(response2, transaction2);

    QCOMPARE(sortedResponses1.size(), 1);
    QCOMPARE(sortedResponses1[0], response1);
    QCOMPARE(sortedResponses2.size(), 1);
    QCOMPARE(sortedResponses2[0], response2);
}

void test_scpi_response_sorter::start_1_2_Response_2_1()
{
    ScpiCmdResponseSorter sorter;
    ScpiTransactionId transaction1 = sorter.createTransaction();
    ScpiTransactionId transaction2 = sorter.createTransaction();

    QString response1 = "1";
    QString response2 = "2";
    QStringList sortedResponses2 = sorter.genOrDelaySortedOutput(response2, transaction2);
    QStringList sortedResponses1 = sorter.genOrDelaySortedOutput(response1, transaction1);

    QCOMPARE(sortedResponses2.size(), 0);
    QCOMPARE(sortedResponses1.size(), 2);
    QCOMPARE(sortedResponses1[0], response1);
    QCOMPARE(sortedResponses1[1], response2);
}

void test_scpi_response_sorter::start_1_2_3_Response_2_1_3()
{
    ScpiCmdResponseSorter sorter;
    ScpiTransactionId transaction1 = sorter.createTransaction();
    ScpiTransactionId transaction2 = sorter.createTransaction();
    ScpiTransactionId transaction3 = sorter.createTransaction();

    QString response1 = "1";
    QString response2 = "2";
    QString response3 = "3";
    QStringList sortedResponses2 = sorter.genOrDelaySortedOutput(response2, transaction2);
    QStringList sortedResponses1 = sorter.genOrDelaySortedOutput(response1, transaction1);
    QStringList sortedResponses3 = sorter.genOrDelaySortedOutput(response3, transaction3);

    QCOMPARE(sortedResponses2.size(), 0);
    QCOMPARE(sortedResponses1.size(), 2);
    QCOMPARE(sortedResponses1[0], response1);
    QCOMPARE(sortedResponses1[1], response2);
    QCOMPARE(sortedResponses3.size(), 1);
    QCOMPARE(sortedResponses3[0], response3);
}

void test_scpi_response_sorter::start_1_2_3_Response_3_1_2()
{
    ScpiCmdResponseSorter sorter;
    ScpiTransactionId transaction1 = sorter.createTransaction();
    ScpiTransactionId transaction2 = sorter.createTransaction();
    ScpiTransactionId transaction3 = sorter.createTransaction();

    QString response1 = "1";
    QString response2 = "2";
    QString response3 = "3";
    QStringList sortedResponses3 = sorter.genOrDelaySortedOutput(response3, transaction3);
    QStringList sortedResponses1 = sorter.genOrDelaySortedOutput(response1, transaction1);
    QStringList sortedResponses2 = sorter.genOrDelaySortedOutput(response2, transaction2);

    QCOMPARE(sortedResponses3.size(), 0);
    QCOMPARE(sortedResponses1.size(), 1);
    QCOMPARE(sortedResponses1[0], response1);
    QCOMPARE(sortedResponses2.size(), 2);
    QCOMPARE(sortedResponses2[0], response2);
    QCOMPARE(sortedResponses2[1], response3);
}

void test_scpi_response_sorter::twoResponsesOnInvalidTransaction()
{
    ScpiCmdResponseSorter sorter;

    QString response1 = "1";
    QString response2 = "2";
    QStringList sortedResponses1 = sorter.genOrDelaySortedOutput(response1, ScpiTransactionId());
    QStringList sortedResponses2 = sorter.genOrDelaySortedOutput(response2, ScpiTransactionId());

    QCOMPARE(sortedResponses1.size(), 1);
    QCOMPARE(sortedResponses1[0], response1);
    QCOMPARE(sortedResponses2.size(), 1);
    QCOMPARE(sortedResponses2[0], response2);
}

void test_scpi_response_sorter::start_2_Response_1_2_3()
{
    ScpiCmdResponseSorter sorter;
    ScpiTransactionId transaction2 = sorter.createTransaction();

    QString response1 = "1";
    QString response2 = "2";
    QString response3 = "3";
    QStringList sortedResponses1 = sorter.genOrDelaySortedOutput(response1, ScpiTransactionId());
    QStringList sortedResponses2 = sorter.genOrDelaySortedOutput(response2, transaction2);
    QStringList sortedResponses3 = sorter.genOrDelaySortedOutput(response3, ScpiTransactionId());

    QCOMPARE(sortedResponses1.size(), 1);
    QCOMPARE(sortedResponses1[0], response1);
    QCOMPARE(sortedResponses2.size(), 1);
    QCOMPARE(sortedResponses2[0], response2);
    QCOMPARE(sortedResponses3.size(), 1);
    QCOMPARE(sortedResponses3[0], response3);
}

void test_scpi_response_sorter::start_1_2_Response_3_2_1()
{
    ScpiCmdResponseSorter sorter;
    ScpiTransactionId transaction1 = sorter.createTransaction();
    ScpiTransactionId transaction2 = sorter.createTransaction();

    QString response1 = "1";
    QString response2 = "2";
    QString response3 = "3";
    QStringList sortedResponses3 = sorter.genOrDelaySortedOutput(response3, ScpiTransactionId());
    QStringList sortedResponses2 = sorter.genOrDelaySortedOutput(response2, transaction2);
    QStringList sortedResponses1 = sorter.genOrDelaySortedOutput(response1, transaction1);

    QCOMPARE(sortedResponses1.size(), 2);
    QCOMPARE(sortedResponses1[0], response1);
    QCOMPARE(sortedResponses1[1], response2);
    QCOMPARE(sortedResponses2.size(), 0);
    QCOMPARE(sortedResponses3.size(), 1);
    QCOMPARE(sortedResponses3[0], response3);
}

void test_scpi_response_sorter::start_1_2_Response_2_1_2_1()
{
    ScpiCmdResponseSorter sorter;
    ScpiTransactionId transaction1 = sorter.createTransaction();
    ScpiTransactionId transaction2 = sorter.createTransaction();

    QString response1a = "1a";
    QString response2a = "2a";
    QStringList sortedResponses2a = sorter.genOrDelaySortedOutput(response2a, transaction2);
    QStringList sortedResponses1a = sorter.genOrDelaySortedOutput(response1a, transaction1);
    QCOMPARE(sortedResponses2a.size(), 0);
    QCOMPARE(sortedResponses1a.size(), 2);
    QCOMPARE(sortedResponses1a[0], response1a);
    QCOMPARE(sortedResponses1a[1], response2a);

    // multiresponse is treated same as invalid
    QString response1b = "1b";
    QString response2b = "2b";
    QStringList sortedResponses2b = sorter.genOrDelaySortedOutput(response2b, transaction2);
    QStringList sortedResponses1b = sorter.genOrDelaySortedOutput(response1b, transaction1);
    QCOMPARE(sortedResponses1b.size(), 1);
    QCOMPARE(sortedResponses1b[0], response1b);
    QCOMPARE(sortedResponses2b.size(), 1);
    QCOMPARE(sortedResponses2b[0], response2b);
}
