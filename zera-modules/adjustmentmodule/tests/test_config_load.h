#ifndef TEST_CONFIG_LOAD_H
#define TEST_CONFIG_LOAD_H

#include <QObject>

class test_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void fileLoaded();
};

#endif // TEST_CONFIG_LOAD_H
