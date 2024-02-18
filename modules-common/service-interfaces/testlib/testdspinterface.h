#ifndef TESTDSPINTERFACE_H
#define TESTDSPINTERFACE_H

#include <mockdspinterface.h>

class TestDspInterface : public MockDspInterface
{
    Q_OBJECT
public:
    TestDspInterface();
};

typedef std::shared_ptr<TestDspInterface> TestDspInterfacePtr;

#endif // TESTDSPINTERFACE_H
