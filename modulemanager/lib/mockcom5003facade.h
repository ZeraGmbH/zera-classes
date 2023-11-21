#ifndef MOCKCOM5003FACADE_H
#define MOCKCOM5003FACADE_H

#include "resmanrunfacade.h"
#include "mockcom5003d.h"
#include "mocksec1000d.h"

class MockCom5003Facade
{
public:
    MockCom5003Facade();
private:
    ResmanRunFacade m_resman;
    MockCom5003d m_mockcom5003d;
    MockSec1000d m_mocksec1000d;
};

#endif // MOCKCOM5003FACADE_H
