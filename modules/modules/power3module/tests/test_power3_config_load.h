#ifndef TEST_POWER3_CONFIG_LOAD_H
#define TEST_POWER3_CONFIG_LOAD_H

#include <QObject>

class test_power3_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
};

#endif // TEST_POWER3_CONFIG_LOAD_H
