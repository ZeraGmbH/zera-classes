#ifndef MOCKMT310S2FACADE_H
#define MOCKMT310S2FACADE_H

#include "resmanrunfacade.h"
#include "mockmt310s2d.h"
#include "mocksec1000d.h"

class MockMt310s2Facade
{
public:
    MockMt310s2Facade();
private:
    ResmanRunFacade m_resman;
    MockMt310s2d m_mockmt310s2d;
    MockSec1000d m_mocksec1000d;
};

#endif // MOCKMT310S2FACADE_H
