#ifndef TEST_SCPI_DOC_PLAYGROUND_H
#define TEST_SCPI_DOC_PLAYGROUND_H

#include <QObject>

class test_scpi_doc_playground : public QObject
{
    Q_OBJECT
private slots:
    void createAlHtmlsExists();
    void runCreateAlHtmls();
    void createDevIfaceXml();
};

#endif // TEST_SCPI_DOC_PLAYGROUND_H
