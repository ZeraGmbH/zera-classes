#ifndef TEST_SCPI_QUEUE_H
#define TEST_SCPI_QUEUE_H

#include <QObject>

class test_scpi_queue : public QObject
{
    Q_OBJECT
private slots:
    void sendStandardCmdsQueueDisabledAndEnabled();
    void sendErroneousAndCorrectStandardCmds();

    void sendSubSystemAndStandardCommands();
    void enableAndDisableQueueWhileExecutingCmds();
    void disableAndEnableQueueWhileExecutingCmds();
};

#endif // TEST_SCPI_QUEUE_H