#ifndef TEST_TRANSFORMER1_CONFIG_LOAD_H
#define TEST_TRANSFORMER1_CONFIG_LOAD_H

#include <QObject>

class test_transformer1_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
    void writtenXmlIsStillValid();
};

#endif // TEST_TRANSFORMER1_CONFIG_LOAD_H
