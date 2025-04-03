#ifndef TEST_SFC_CONFIG_LOAD_H
#define TEST_SFC_CONFIG_LOAD_H

#include <QObject>

class test_sfc_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
};

#endif // TEST_SFC_CONFIG_LOAD_H
