#ifndef TEST_ADJ_MODULE_PHASE_H
#define TEST_ADJ_MODULE_PHASE_H

#include <QObject>

class test_adj_module_phase : public QObject
{
    Q_OBJECT
private slots:
    void noActValuesWithPermission();
    void validActValuesWithPermission();
    void noActValuesWithoutPermission();
};

#endif // TEST_ADJ_MODULE_PHASE_H
