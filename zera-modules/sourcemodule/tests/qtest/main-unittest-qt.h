#ifndef MAIN_UNITTEST_H
#define MAIN_UNITTEST_H

#include "test_globals.h"
#include <QObject>
#include <QList>
#include <QString>
#include <vs_veinhash.h>

typedef int (*Ttest)(int argc, char *argv[]);
Ttest addTestFunc(Ttest ptr);

#undef QTEST_MAIN
#define QTEST_MAIN(TestClass) \
    static int testFunc(int argc, char *argv[]) \
    { \
        QTEST_MAIN_IMPL(TestClass) \
    } \
    static Ttest dummy = addTestFunc(testFunc);



#endif // MAIN_UNITTEST_H
