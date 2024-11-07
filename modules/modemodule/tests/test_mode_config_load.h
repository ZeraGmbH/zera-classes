#ifndef TEST_MODE_CONFIG_LOAD_H
#define TEST_MODE_CONFIG_LOAD_H

#include <QObject>

class test_mode_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
    void writtenXmlIsStillValid();
};

#endif // TEST_MODE_CONFIG_LOAD_H
