#ifndef ZERAMCONTROLLERBOOTLOADERSTOPPERFACTORYFORTEST_H
#define ZERAMCONTROLLERBOOTLOADERSTOPPERFACTORYFORTEST_H

#include "zeramcontrollerbootloaderstopperfactory.h"
#include "zeramcontrollerio.h"
#include <QVector>

class ZeraMControllerBootloaderStopperFactoryForTest : public ZeraMControllerBootloaderStopperFactory
{
public:
    static void setBootoaderAssumeAppStartedImmediates(QVector<bool> assumeAppStartedImmediates);
    static bool checkEmpty();
private:
    static QVector<bool> m_assumeAppStartedImmediates;
};

#endif // ZERAMCONTROLLERBOOTLOADERSTOPPERFACTORYFORTEST_H
