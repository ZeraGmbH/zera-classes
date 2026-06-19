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


    QByteArray response = "1";
    QByteArray sortedResponse = sorter.genOrDelaySortedOutput(response, transaction);

    QCOMPARE(sortedResponse, response);
}

void test_scpi_response_sorter::start_1_2_Response_1_2()
{
    ScpiCmdResponseSorter sorter;
    ScpiTransactionId transaction1 = sorter.createTransaction();
    ScpiTransactionId transaction2 = sorter.createTransaction();

    QByteArray response1 = "1";
    QByteArray response2 = "2";
    QByteArray sortedResponse1 = sorter.genOrDelaySortedOutput(response1, transaction1);
    QByteArray sortedResponse2 = sorter.genOrDelaySortedOutput(response2, transaction2);

    QCOMPARE(sortedResponse1, response1);
    QCOMPARE(sortedResponse2, response2);
}

void test_scpi_response_sorter::start_1_2_Response_2_1()
{
    ScpiCmdResponseSorter sorter;
    ScpiTransactionId transaction1 = sorter.createTransaction();
    ScpiTransactionId transaction2 = sorter.createTransaction();

    QByteArray response1 = "1";
    QByteArray response2 = "2";
    QByteArray sortedResponse2 = sorter.genOrDelaySortedOutput(response2, transaction2);
    QByteArray sortedResponse1 = sorter.genOrDelaySortedOutput(response1, transaction1);

    QCOMPARE(sortedResponse2, "");
    QCOMPARE(sortedResponse1, response1 + response2);
}

void test_scpi_response_sorter::start_1_2_3_Response_2_1_3()
{
    ScpiCmdResponseSorter sorter;
    ScpiTransactionId transaction1 = sorter.createTransaction();
    ScpiTransactionId transaction2 = sorter.createTransaction();
    ScpiTransactionId transaction3 = sorter.createTransaction();

    QByteArray response1 = "1";
    QByteArray response2 = "2";
    QByteArray response3 = "3";
    QByteArray sortedResponse2 = sorter.genOrDelaySortedOutput(response2, transaction2);
    QByteArray sortedResponse1 = sorter.genOrDelaySortedOutput(response1, transaction1);
    QByteArray sortedResponse3 = sorter.genOrDelaySortedOutput(response3, transaction3);

    QCOMPARE(sortedResponse2, "");
    QCOMPARE(sortedResponse1, response1 + response2);
    QCOMPARE(sortedResponse3, response3);
}

void test_scpi_response_sorter::twoResponsesOnInvalidTransaction()
{
    ScpiCmdResponseSorter sorter;

    QByteArray response1 = "1";
    QByteArray response2 = "2";
    QByteArray sortedResponse1 = sorter.genOrDelaySortedOutput(response1, ScpiTransactionId());
    QByteArray sortedResponse2 = sorter.genOrDelaySortedOutput(response2, ScpiTransactionId());

    QCOMPARE(sortedResponse1, response1);
    QCOMPARE(sortedResponse2, response2);
}

void test_scpi_response_sorter::start_2_Response_1_2_3()
{
    ScpiCmdResponseSorter sorter;
    ScpiTransactionId transaction2 = sorter.createTransaction();

    QByteArray response1 = "1";
    QByteArray response2 = "2";
    QByteArray response3 = "3";
    QByteArray sortedResponse1 = sorter.genOrDelaySortedOutput(response1, ScpiTransactionId());
    QByteArray sortedResponse2 = sorter.genOrDelaySortedOutput(response2, transaction2);
    QByteArray sortedResponse3 = sorter.genOrDelaySortedOutput(response3, ScpiTransactionId());

    QCOMPARE(sortedResponse1, response1);
    QCOMPARE(sortedResponse2, response2);
    QCOMPARE(sortedResponse3, response3);
}

void test_scpi_response_sorter::start_1_2_Response_3_2_1()
{
    ScpiCmdResponseSorter sorter;
    ScpiTransactionId transaction1 = sorter.createTransaction();
    ScpiTransactionId transaction2 = sorter.createTransaction();

    QByteArray response1 = "1";
    QByteArray response2 = "2";
    QByteArray response3 = "3";
    QByteArray sortedResponse3 = sorter.genOrDelaySortedOutput(response3, ScpiTransactionId());
    QByteArray sortedResponse2 = sorter.genOrDelaySortedOutput(response2, transaction2);
    QByteArray sortedResponse1 = sorter.genOrDelaySortedOutput(response1, transaction1);

    QCOMPARE(sortedResponse1, response1+response2);
    QCOMPARE(sortedResponse2, "");
    QCOMPARE(sortedResponse3, response3);
}

void test_scpi_response_sorter::start_1_2_Response_2_1_2_1()
{
    ScpiCmdResponseSorter sorter;
    ScpiTransactionId transaction1 = sorter.createTransaction();
    ScpiTransactionId transaction2 = sorter.createTransaction();

    QByteArray response1a = "1a";
    QByteArray response2a = "2a";
    QByteArray sortedResponse2a = sorter.genOrDelaySortedOutput(response2a, transaction2);
    QByteArray sortedResponse1a = sorter.genOrDelaySortedOutput(response1a, transaction1);
    QCOMPARE(sortedResponse2a, "");
    QCOMPARE(sortedResponse1a, response1a+response2a);

    // multiresponse is treated same as invalid
    QByteArray response1b = "1b";
    QByteArray response2b = "2b";
    QByteArray sortedResponse2b = sorter.genOrDelaySortedOutput(response2b, transaction2);
    QByteArray sortedResponse1b = sorter.genOrDelaySortedOutput(response1b, transaction1);
    QCOMPARE(sortedResponse1b, response1b);
    QCOMPARE(sortedResponse2b, response2b);
}
