#ifndef TEST_DFT_CONFIG_LOAD_H
#define TEST_DFT_CONFIG_LOAD_H

#include <QObject>

class test_dft_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
};

#endif // TEST_DFT_CONFIG_LOAD_H
