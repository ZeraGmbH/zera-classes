#ifndef TEST_POW2_CONFIG_LOAD_H
#define TEST_POW2_CONFIG_LOAD_H

#include <QObject>

class test_pow2_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
};

#endif // TEST_POW2_CONFIG_LOAD_H
