#ifndef TEST_EFFICIENCY1_CONFIG_LOAD_H
#define TEST_EFFICIENCY1_CONFIG_LOAD_H

#include <QObject>

class test_efficiency1_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
};

#endif // TEST_EFFICIENCY1_CONFIG_LOAD_H
