#ifndef MOCKMT310S2FACADE_H
#define MOCKMT310S2FACADE_H

#include "resmanrunfacade.h"
#include "mockmt310s2dfull.h"
#include "mocksec1000d.h"
#include "mockzdsp1d.h"

class MockMt310s2Facade
{
public:
    MockMt310s2Facade();
    virtual ~MockMt310s2Facade();
private:
    ResmanRunFacade *m_resman;
    MockMt310s2dFull *m_mockmt310s2d;
    MockSec1000d *m_mocksec1000d;
    MockZdsp1d *m_mockzdsp1d;
};

#endif // MOCKMT310S2FACADE_H
