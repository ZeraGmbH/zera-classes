#ifndef TEST_MEASMODEPHASEPERSISTENCY_H
#define TEST_MEASMODEPHASEPERSISTENCY_H

#include <QObject>

class test_measmodephasepersistency : public QObject
{
    Q_OBJECT
private slots:
    void readUnmatchingMode();
    void readMatchingMode();
    void failOnMissingComma();
    void failOnInvalidMask();
    void changeOneMaskProperly();
    void changeTwoMasksProperly();
    void failOnFixedMask();
};

#endif // TEST_MEASMODEPHASEPERSISTENCY_H
