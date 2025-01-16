#ifndef TEST_SCPI_DOC_GEN_SMOKE_H
#define TEST_SCPI_DOC_GEN_SMOKE_H

#include <QObject>

class test_scpi_doc_gen_smoke : public QObject
{
    Q_OBJECT
private slots:
    void generateAllDocs();
};

#endif // TEST_SCPI_DOC_GEN_SMOKE_H
