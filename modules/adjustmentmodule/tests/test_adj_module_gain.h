#ifndef TEST_ADJ_MODULE_GAIN_H
#define TEST_ADJ_MODULE_GAIN_H

#include <QObject>

class test_adj_module_gain : public QObject
{
    Q_OBJECT
private slots:
    void noActValuesWithPermission();
    void noActValuesWithoutPermission();
};

#endif // TEST_ADJ_MODULE_GAIN_H
