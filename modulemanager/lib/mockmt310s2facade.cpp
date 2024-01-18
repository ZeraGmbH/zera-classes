#include "mockmt310s2facade.h"
#include <demoeventloopfeeder.h>

MockMt310s2Facade::MockMt310s2Facade()
{
    m_resman = new ResmanRunFacade;
    DemoEventLoopFeeder::feedEventLoop();
    m_mockmt310s2d = new MockMt310s2dFull;
    m_mocksec1000d = new MockSec1000d;
    m_mockzdsp1d = new MockZdsp1d;
    DemoEventLoopFeeder::feedEventLoop();
}

MockMt310s2Facade::~MockMt310s2Facade()
{
    delete m_mockzdsp1d;
    delete m_mocksec1000d;
    delete m_mockmt310s2d;
    DemoEventLoopFeeder::feedEventLoop();
    delete m_resman;
    DemoEventLoopFeeder::feedEventLoop();
}
