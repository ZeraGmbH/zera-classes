#ifndef TEST_SOURCESWITCHJSONANGLEADJUSTMENTDECORATOR_H
#define TEST_SOURCESWITCHJSONANGLEADJUSTMENTDECORATOR_H

#include <QObject>
#include <vs_abstractcomponent.h>

class test_sourceswitchjsonangleadjustmentdecorator : public QObject
{
    Q_OBJECT
private slots:
    void initialLoadpoint();
    void switchOn();

private:
    static QList<VeinStorage::AbstractComponentPtr> createVeinDftValuesEmpty(int phaseCount);
};

#endif // TEST_SOURCESWITCHJSONANGLEADJUSTMENTDECORATOR_H
