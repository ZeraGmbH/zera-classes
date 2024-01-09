#ifndef TEST_MODMAN_CONFIG_H
#define TEST_MODMAN_CONFIG_H

#include <QObject>

class test_modman_config : public QObject
{
    Q_OBJECT
private slots:
    void loadDevConfig();
    void verifySessionsCount();
};

#endif // TEST_MODMAN_CONFIG_H
