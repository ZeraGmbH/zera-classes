#ifndef TESTDSPINTERFACE_H
#define TESTDSPINTERFACE_H

#include <mockdspinterface.h>
#include <QJsonObject>
#include <QJsonArray>

class TestDspInterface : public MockDspInterface
{
    Q_OBJECT
public:
    TestDspInterface(QStringList valueNamesList);
    QStringList getValueList();

    QJsonObject dumpAll();
    QJsonObject dumpMemoryGroups();
    QJsonObject dumpVarList(QJsonObject inData);
    QStringList dumpCycListItem();
private:
    QString dspVarDataTypeToJson(int type);
    QString dspVarSegmentToJson(int segment);
    QStringList m_valueNamesList;
};

typedef std::shared_ptr<TestDspInterface> TestDspInterfacePtr;

#endif // TESTDSPINTERFACE_H
