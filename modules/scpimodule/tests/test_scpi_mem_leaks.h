#ifndef TEST_SCPI_MEM_LEAKS_H
#define TEST_SCPI_MEM_LEAKS_H

#include "modulemanagertestrunner.h"
#include <QObject>

class test_scpi_mem_leaks : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void memLeakSorted(); // skipped - just for analysis

    void checkLeftoverObjectsMeasureAndFriendsSorted();
    void checkLeftoverObjectsMeasureAndFriendsUnsorted();

private:
    bool runScpiIo(ModuleManagerTestRunner &testRunner);
};

#endif // TEST_SCPI_MEM_LEAKS_H
