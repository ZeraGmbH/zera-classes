#ifndef TEST_RMS_CONFIG_LOAD_H
#define TEST_RMS_CONFIG_LOAD_H

#include <QObject>

class test_rms_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
};

#endif // TEST_RMS_CONFIG_LOAD_H
