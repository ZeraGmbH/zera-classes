#include "main-unittest-qt.h"

static QList<Ttest> listTestFuncs;

Ttest addTestFunc(Ttest ptr)
{
    listTestFuncs.append(ptr);
    return ptr;
}

int main(int argc, char *argv[])
{
    int status = 0;
    Q_INIT_RESOURCE(resource);

    // Make qCritical in tests for re-used IoTransferDataSingle objects - see
    // IoDeviceBase::prepareSendAndReceive
    qputenv("QT_FATAL_CRITICALS", "1");

    while (!listTestFuncs.isEmpty()) {
        auto testFunc = listTestFuncs.takeLast();
        status |= testFunc(argc, argv);
    }
    if(status) {
#ifdef QT_DEBUG
        qWarning("\n\033[1;31m%s\033[0m","One or more tests failed!!!");
#else
        qWarning("\nOne or more tests failed!!!");
#endif
    }
    else {
#ifdef QT_DEBUG
        qInfo("\n\033[1;32m%s\033[0m","All tests passed :)");
#else
        qInfo("\nAll tests passed :)");
#endif
    }
    return status;
}

