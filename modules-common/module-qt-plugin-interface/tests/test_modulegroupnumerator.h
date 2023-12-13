#ifndef TEST_MODULEGROUPNUMERATOR_H
#define TEST_MODULEGROUPNUMERATOR_H

#include <QObject>

class test_modulegroupnumerator : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void requestThreeZeros();

};

#endif // TEST_MODULEGROUPNUMERATOR_H
