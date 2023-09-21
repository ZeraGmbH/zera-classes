#ifndef TEST_SOURCE_CONFIG_LOAD_H
#define TEST_SOURCE_CONFIG_LOAD_H

#include <QObject>

class test_source_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
};

#endif // TEST_SOURCE_CONFIG_LOAD_H
