#ifndef TEST_SAMPLE_CONFIG_LOAD_H
#define TEST_SAMPLE_CONFIG_LOAD_H

#include <QObject>

class test_sample_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
    void writtenXmlIsStillValid();
};

#endif // TEST_SAMPLE_CONFIG_LOAD_H
