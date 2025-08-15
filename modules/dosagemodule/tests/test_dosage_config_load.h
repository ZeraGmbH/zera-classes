#ifndef TEST_DOSAGE_CONFIG_LOAD_H
#define TEST_DOSAGE_CONFIG_LOAD_H

#include <QObject>

class test_dosage_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
    void allFilesFormatCheckLight();
};

#endif // TEST_DOSAGE_CONFIG_LOAD_H
