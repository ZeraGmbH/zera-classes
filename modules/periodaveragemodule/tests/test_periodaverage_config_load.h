#ifndef TEST_PERIODAVERAGE_CONFIG_LOAD_H
#define TEST_PERIODAVERAGE_CONFIG_LOAD_H

#include <QObject>

class test_periodaverage_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
    void writtenXmlIsStillValid();
};

#endif // TEST_PERIODAVERAGE_CONFIG_LOAD_H
