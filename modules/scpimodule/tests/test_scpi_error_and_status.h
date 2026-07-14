#ifndef TEST_SCPI_ERROR_AND_STATUS_H
#define TEST_SCPI_ERROR_AND_STATUS_H

#include "modulemanagertestrunner.h"
#include "scpimodule.h"
#include "scpitestclient.h"
#include <QObject>

class test_scpi_error_and_status : public QObject
{
    Q_OBJECT
public: // public for moc
    enum SortTypes {
        SORTED,
        NOT_SORTED
    };
    Q_ENUM(SortTypes)

private slots:
    void initTestCase();
    void initTestCase_data();

    void sendBullshitQueryStb();
    void sendBullshitTwiceQueryErrorCount();
    void sendValidCommandWithoutSemicolonQueryStb();
    void overflowErrorQueue();
    void causeErrorAndClearIt();

    void noErrorCrossTalkOnMultipleClients();
    void noQuestionableEnableCrossTalkOnMultipleClients();

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
    SCPIMODULE::cSCPIModule *getScpiModule();
    QString sendReceive(SCPIMODULE::ScpiTestClient &client, const QString &scpi);
    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
};

#endif // TEST_SCPI_ERROR_AND_STATUS_H
