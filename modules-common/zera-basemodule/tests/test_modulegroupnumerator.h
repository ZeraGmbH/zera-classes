#ifndef TEST_MODULEGROUPNUMERATOR_H
#define TEST_MODULEGROUPNUMERATOR_H

#include <QObject>

class test_modulegroupnumerator : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void test_add123();
    void test_add11_error();
    void test_add1remove1add1();
    void test_add1remove2add1_error();
    void test_add12remove1add1();
};

#endif // TEST_MODULEGROUPNUMERATOR_H
