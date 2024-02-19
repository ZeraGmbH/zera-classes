#ifndef TESTDSPINTERFACE_H
#define TESTDSPINTERFACE_H

#include <mockdspinterface.h>

class TestDspInterface : public MockDspInterface
{
    Q_OBJECT
public:
    TestDspInterface(QStringList valueNamesList);
    QStringList getValueList();
private:
    QStringList m_valueNamesList;
};

typedef std::shared_ptr<TestDspInterface> TestDspInterfacePtr;

#endif // TESTDSPINTERFACE_H
