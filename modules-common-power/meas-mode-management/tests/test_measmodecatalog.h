#ifndef TEST_MEASMODECATALOG_H
#define TEST_MEASMODECATALOG_H

#include <QObject>

class test_measmodecatalog : public QObject
{
    Q_OBJECT
private slots:
    void invalidIdReturnsInvalid();
    void invalidStringReturnsInvalid();
    void allModesHaveACatalogEntry();
    void reminderForThreeWire();
};

#endif // TEST_MEASMODECATALOG_H
