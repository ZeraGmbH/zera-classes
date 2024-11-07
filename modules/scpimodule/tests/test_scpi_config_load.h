#ifndef TEST_SCPI_CONFIG_LOAD_H
#define TEST_SCPI_CONFIG_LOAD_H

#include <QObject>

class test_scpi_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
    void writtenXmlIsStillValid();
};

#endif // TEST_SCPI_CONFIG_LOAD_H
