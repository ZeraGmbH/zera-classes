#ifndef TEST_SCPI_ERROR_AND_STATUS_H
#define TEST_SCPI_ERROR_AND_STATUS_H

#include "modulemanagertestrunner.h"
#include <QObject>

class test_scpi_error_and_status : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void sendBullshitQueryStb();
    void sendBullshitTwiceQueryErrorCount();
    void sendValidCommandWithoutSemicolonQueryStb();
    void overflowErrorQueue();
    void causeErrorAndClearIt();

    void causeCommandErrorOpc();
    void causeCommandErrorEse();
    void causeCommandErrorSre();
    void causeCommandErrorCls();
    void causeQueryErrorCls();
    void causeCommandErrorRst();
    void causeQueryErrorRst();
    void causeCommandErrorIdn();
    void causeCommandErrorEsr();
    void causeCommandErrorStb();
    void causeCommandErrorTst();
    void causeCommandErrorReadError();
    void causeCommandErrorReadErrorCount();
    void causeCommandErrorReadAllErrors();

    void sendStatusQuestionableEnableInvalidParam();

private:
    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
};

#endif // TEST_SCPI_ERROR_AND_STATUS_H
