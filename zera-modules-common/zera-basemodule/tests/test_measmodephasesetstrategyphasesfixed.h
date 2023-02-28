#ifndef TEST_MEASMODEPHASESETSTRATEGYPHASESFIXED_H
#define TEST_MEASMODEPHASESETSTRATEGYPHASESFIXED_H

#include <QObject>

class test_measmodephasesetstrategyphasesfixed : public QObject
{
    Q_OBJECT
private slots:
    void canChangeMask();
    void phaseChangeDeny();
    void maskChangeDeny();
    void maskInitialValue();
};

#endif // TEST_MEASMODEPHASESETSTRATEGYPHASESFIXED_H
