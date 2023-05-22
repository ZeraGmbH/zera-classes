#include "zeramcontrollerbootloaderstopperfactoryfortest.h"
#include "zeramcontrollerbootloaderstopperfortest.h"

QVector<bool> ZeraMControllerBootloaderStopperFactoryForTest::m_assumeAppStartedImmediates;

void ZeraMControllerBootloaderStopperFactoryForTest::setBootoaderAssumeAppStartedImmediates(QVector<bool> assumeAppStartedImmediates)
{
    if(!checkEmpty())
        qFatal("m_assumeAppStartedImmediates not empty!");
    m_assumeAppStartedImmediates = assumeAppStartedImmediates;
    m_createFunction =
            [](ZeraMcontrollerIoPtr i2cCtrl, int channelId)
    {
        Q_UNUSED(i2cCtrl)
        return std::make_shared<ZeraMControllerBootloaderStopperForTest>(channelId, m_assumeAppStartedImmediates.takeFirst());
    };
}

bool ZeraMControllerBootloaderStopperFactoryForTest::checkEmpty()
{
    return m_assumeAppStartedImmediates.isEmpty();
}
