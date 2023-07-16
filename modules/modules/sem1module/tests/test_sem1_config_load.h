#ifndef TEST_SEM1_CONFIG_LOAD_H
#define TEST_SEM1_CONFIG_LOAD_H

#include <QObject>

class test_sem1_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
};

#endif // TEST_SEM1_CONFIG_LOAD_H
