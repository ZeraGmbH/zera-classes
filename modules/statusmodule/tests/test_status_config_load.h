#ifndef TEST_STATUS_CONFIG_LOAD_H
#define TEST_STATUS_CONFIG_LOAD_H

#include <QObject>

class test_status_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void fileLoaded();
    void writtenXmlIsStillValid();
};

#endif // TEST_STATUS_CONFIG_LOAD_H
