#ifndef TEST_ADJ_CONFIG_LOAD_H
#define TEST_ADJ_CONFIG_LOAD_H

#include <QObject>

class test_adj_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void fileLoaded();
};

#endif // TEST_ADJ_CONFIG_LOAD_H
