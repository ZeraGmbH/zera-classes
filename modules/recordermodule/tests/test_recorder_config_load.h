#ifndef TEST_RECORDER_CONFIG_LOAD_H
#define TEST_RECORDER_CONFIG_LOAD_H

#include <QObject>

class test_recorder_config_load : public QObject
{
    Q_OBJECT
private slots:
    void allFilesLoaded();
    void writtenXmlIsStillValid();
};

#endif // TEST_RECORDER_CONFIG_LOAD_H
