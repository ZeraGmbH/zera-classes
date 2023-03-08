#ifndef TEST_MEASMODEPHASESETSTRATEGYXWIRE_H
#define TEST_MEASMODEPHASESETSTRATEGYXWIRE_H

#include <QObject>

class test_measmodephasesetstrategyxwire : public QObject
{
    Q_OBJECT
private slots:
    void activePhasesAfterInit();
    void activePhasesAfterChange();
};

#endif // TEST_MEASMODEPHASESETSTRATEGYXWIRE_H
