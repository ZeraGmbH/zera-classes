#ifndef TEST_SEC1_CONFIG_LOAD_H
#define TEST_SEC1_CONFIG_LOAD_H

#include <QObject>

class test_sec1_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
    void writtenXmlIsStillValid();
};

#endif // TEST_SEC1_CONFIG_LOAD_H
