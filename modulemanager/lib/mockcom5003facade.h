#ifndef MOCKCOM5003FACADE_H
#define MOCKCOM5003FACADE_H

#include "resmanrunfacade.h"
#include "mockcom5003dfull.h"
#include "mocksec1000d.h"
#include "mockzdsp1d.h"

class MockCom5003Facade
{
public:
    MockCom5003Facade();
    virtual ~MockCom5003Facade();
private:
    ResmanRunFacade *m_resman;
    MockCom5003dFull *m_mockcom5003d;
    MockSec1000d *m_mocksec1000d;
    MockZdsp1d *m_mockzdsp1d;
};

#endif // MOCKCOM5003FACADE_H
