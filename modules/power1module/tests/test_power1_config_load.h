#ifndef TEST_POW1_CONFIG_LOAD_H
#define TEST_POW1_CONFIG_LOAD_H

#include <QObject>

class test_power1_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
    void modeListCountSameAsArraySize();
    void modePhaseListCountSameAsArraySize();
    void modePhaseListPlausis();
    void measSystemsValid();
    void allModesProperlyLoadedFromConfig();
};

#endif // TEST_POW1_CONFIG_LOAD_H
