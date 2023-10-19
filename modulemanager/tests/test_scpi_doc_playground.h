#ifndef TEST_SCPI_DOC_PLAYGROUND_H
#define TEST_SCPI_DOC_PLAYGROUND_H

#include <QObject>

class test_scpi_doc_playground : public QObject
{
    Q_OBJECT
private slots:
    void createAlHtmlsExists();
    void createMt310s2Xmls();
    void createCom5003Xmls();
    void runCreateAlHtmls();
private:
    void generateDeviceXmls(QString deviceName);
};

#endif // TEST_SCPI_DOC_PLAYGROUND_H
