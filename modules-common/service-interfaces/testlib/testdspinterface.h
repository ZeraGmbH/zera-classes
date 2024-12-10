#ifndef TESTDSPINTERFACE_H
#define TESTDSPINTERFACE_H

#include <mockdspinterface.h>

class TestDspInterface : public MockDspInterface
{
    Q_OBJECT
public:
    TestDspInterface(QStringList valueNamesList);
    QStringList getValueList();

    void addCycListItem(QString cmd) override;
    void addCycListItems(const QStringList &cmds) override;

    QByteArray dumpCycListItem();
private:
    QStringList m_valueNamesList;
    QStringList m_cyclicListItems;
};

typedef std::shared_ptr<TestDspInterface> TestDspInterfacePtr;

#endif // TESTDSPINTERFACE_H
