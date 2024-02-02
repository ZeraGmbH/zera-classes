#ifndef TEST_THDN_CONFIG_LOAD_H
#define TEST_THDN_CONFIG_LOAD_H

#include <QObject>

class test_thdn_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
};

#endif // TEST_THDN_CONFIG_LOAD_H
