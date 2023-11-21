#include "mockcom5003facade.h"

MockCom5003Facade::MockCom5003Facade(QString sysroot) :
    m_mockcom5003d(sysroot),
    m_mocksec1000d(sysroot)
{
}
