#ifndef TEST_BLE_CONFIG_LOAD_H
#define TEST_BLE_CONFIG_LOAD_H

#include <QObject>

class test_ble_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
};

#endif // TEST_BLE_CONFIG_LOAD_H
