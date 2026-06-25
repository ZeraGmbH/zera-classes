#ifndef TEST_SCPI_RESPONSE_SORTER_H
#define TEST_SCPI_RESPONSE_SORTER_H

#include <QObject>

class test_scpi_response_sorter : public QObject
{
    Q_OBJECT
private slots:
    void queriesReturnValidId();
    void commandsReturnInvalidId();
    void queryTransactionsChrono();
    void start_1_Response_1();
    void start_1_2_Response_1_2();
    void start_1_2_Response_2_1();
    void start_1_2_3_Response_2_1_3();
    void start_1_2_3_Response_3_1_2();
    void twoResponsesOnInvalidTransaction();
    void start_2_Response_1_2_3();
    void start_1_2_Response_3_2_1();

    void start_1_2_Response_2_1_2_1();
};

#endif // TEST_SCPI_RESPONSE_SORTER_H
