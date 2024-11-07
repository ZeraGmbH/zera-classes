#ifndef TEST_SPM1_CONFIG_LOAD_H
#define TEST_SPM1_CONFIG_LOAD_H

#include <QObject>

class test_spm1_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
    void writtenXmlIsStillValid();
};

#endif // TEST_SPM1_CONFIG_LOAD_H
