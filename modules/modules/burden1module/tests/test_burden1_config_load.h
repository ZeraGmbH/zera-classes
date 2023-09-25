#ifndef TEST_BURDEN1_CONFIG_LOAD_H
#define TEST_BURDEN1_CONFIG_LOAD_H

#include <QObject>

class test_burden1_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
};

#endif // TEST_BURDEN1_CONFIG_LOAD_H
