#include "mockcom5003facade.h"
#include <demoeventloopfeeder.h>

MockCom5003Facade::MockCom5003Facade()
{
    m_resman = new ResmanRunFacade;
    DemoEventLoopFeeder::feedEventLoop();
    m_mockcom5003d = new MockCom5003dFull;
    m_mocksec1000d = new MockSec1000d;
    m_mockzdsp1d = new MockZdsp1d;
    DemoEventLoopFeeder::feedEventLoop();
}

MockCom5003Facade::~MockCom5003Facade()
{
    delete m_mockzdsp1d;
    delete m_mocksec1000d;
    delete m_mockcom5003d;
    DemoEventLoopFeeder::feedEventLoop();
    delete m_resman;
    DemoEventLoopFeeder::feedEventLoop();
}
