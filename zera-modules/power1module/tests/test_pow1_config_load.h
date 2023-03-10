#ifndef TEST_POW1_CONFIG_LOAD_H
#define TEST_POW1_CONFIG_LOAD_H

#include <QObject>

class test_pow1_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
    void modePhaseListCountSameAsArraySize();
    void modePhaseListPlausis();
};

#endif // TEST_POW1_CONFIG_LOAD_H
