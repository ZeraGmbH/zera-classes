#include "zeramcontrolleriofortest.h"

ZeraMcontrollerIoForTest::ZeraMcontrollerIoForTest(QString devnode, quint8 adr, quint8 debuglevel) :
    ZeraMControllerIoTemplate(devnode, adr, debuglevel)
{
}

void ZeraMcontrollerIoForTest::simulateApplicationRunnung()
{
    m_return = cmdfault;
}

void ZeraMcontrollerIoForTest::simulateBooloaderRunning()
{
    m_return = cmddone;
}

ZeraMControllerIoTemplate::atmelRM ZeraMcontrollerIoForTest::bootloaderStartProgram()
{
    return m_return;
}
