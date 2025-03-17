#ifndef TEST_MODMAN_CONFIG_H
#define TEST_MODMAN_CONFIG_H

#include <QObject>

class test_modman_config : public QObject
{
    Q_OBJECT
private slots:
    void loadConfig();
    void verifySessionsCount();
    void pcbServiceConnectionInfo();
    void dspServiceConnectionInfo();
    void secServiceConnectionInfo();
    void resmanServiceConnectionInfo();
private:
    static QString getConfiguredIp();
};

#endif // TEST_MODMAN_CONFIG_H
