#include "test_scpi_response_sorter.h"
#include "scpiresponsesorter.h"
#include <QTest>

QTEST_MAIN(test_scpi_response_sorter)

static constexpr const char* dummyScpiQuery = "foo?";

void test_scpi_response_sorter::queriesReturnValidId()
{
    ScpiResponseSorter sorter;

    QCOMPARE(sorter.createTransaction(dummyScpiQuery).isValid(), true);
    QCOMPARE(sorter.createTransaction(dummyScpiQuery).isValid(), true);
    QCOMPARE(sorter.createTransaction(dummyScpiQuery).isValid(), true);
}

void test_scpi_response_sorter::commandsReturnInvalidId()
{
    ScpiResponseSorter sorter;

    QCOMPARE(sorter.createTransaction("SENS:RNG1:UL1:RANG 250V;").isValid(), false);
}

void test_scpi_response_sorter::queryTransactionsChrono()
{
    ScpiResponseSorter sorter;
    ScpiTransactionId id1 = sorter.createTransaction(dummyScpiQuery);
    ScpiTransactionId id2 = sorter.createTransaction(dummyScpiQuery);
    ScpiTransactionId id3 = sorter.createTransaction(dummyScpiQuery);

    QVERIFY(id1.getChrono() < id2.getChrono());
    QVERIFY(id2.getChrono() < id3.getChrono());
}

void test_scpi_response_sorter::start_1_Response_1()
{
    ScpiResponseSorter sorter;
    ScpiTransactionId transaction = sorter.createTransaction(dummyScpiQuery);

    QString response = "1";
    QStringList sortedResponses = sorter.genOrDelaySortedOutput(response, transaction);

    QCOMPARE(sortedResponses.size(), 1);
    QCOMPARE(sortedResponses[0], response);
}

void test_scpi_response_sorter::start_1_2_Response_1_2()
{
    ScpiResponseSorter sorter;
    ScpiTransactionId transaction1 = sorter.createTransaction(dummyScpiQuery);
    ScpiTransactionId transaction2 = sorter.createTransaction(dummyScpiQuery);

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
    ScpiResponseSorter sorter;
    ScpiTransactionId transaction1 = sorter.createTransaction(dummyScpiQuery);
    ScpiTransactionId transaction2 = sorter.createTransaction(dummyScpiQuery);

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
    ScpiResponseSorter sorter;
    ScpiTransactionId transaction1 = sorter.createTransaction(dummyScpiQuery);
    ScpiTransactionId transaction2 = sorter.createTransaction(dummyScpiQuery);
    ScpiTransactionId transaction3 = sorter.createTransaction(dummyScpiQuery);

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
    ScpiResponseSorter sorter;
    ScpiTransactionId transaction1 = sorter.createTransaction(dummyScpiQuery);
    ScpiTransactionId transaction2 = sorter.createTransaction(dummyScpiQuery);
    ScpiTransactionId transaction3 = sorter.createTransaction(dummyScpiQuery);

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
    ScpiResponseSorter sorter;

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
    ScpiResponseSorter sorter;
    ScpiTransactionId transaction2 = sorter.createTransaction(dummyScpiQuery);

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
    ScpiResponseSorter sorter;
    ScpiTransactionId transaction1 = sorter.createTransaction(dummyScpiQuery);
    ScpiTransactionId transaction2 = sorter.createTransaction(dummyScpiQuery);

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
    ScpiResponseSorter sorter;
    ScpiTransactionId transaction1 = sorter.createTransaction(dummyScpiQuery);
    ScpiTransactionId transaction2 = sorter.createTransaction(dummyScpiQuery);

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
