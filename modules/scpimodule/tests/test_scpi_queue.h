#ifndef TEST_SCPI_QUEUE_H
#define TEST_SCPI_QUEUE_H

#include "abstractfactoryactvalmaninthemiddle.h"
#include <QObject>

class test_scpi_queue : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void sendStandardCmdsQueueDisabledAndEnabled();
    void sendErroneousAndCorrectStandardCmds();

    void sendSubSystemAndStandardCommands();
    void enableAndDisableQueueWhileExecutingCmds();
    void disableAndEnableQueueWhileExecutingCmds();
private:
    QByteArray loadConfig(QString configFileNameFull);
    AbstractFactoryActValManInTheMiddlePtr m_actValGen;

};

#endif // TEST_SCPI_QUEUE_H
