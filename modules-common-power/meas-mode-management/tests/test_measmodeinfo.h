#ifndef TEST_MEASMODEINFO_H
#define TEST_MEASMODEINFO_H

#include <QObject>

class test_measmodeinfo : public QObject
{
    Q_OBJECT
private slots:
    void fourWireIsNotThreeWire();
    void threeWireActiveIsThreeWire();
    void threeWireReactiveIsThreeWire();
    void threeWireApparentIsThreeWire();
};

#endif // TEST_MEASMODEINFO_H
