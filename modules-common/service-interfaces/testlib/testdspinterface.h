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
    quint32 dspMemoryWrite(cDspMeasData* memgroup) override;

    QStringList getValueList();
    QJsonObject dumpAll(bool dumpVarWrite = false);
    QJsonObject dumpMemoryGroups();
    QJsonObject dumpVarList(QJsonObject inData);
    QStringList dumpCycListItem();
    QJsonObject dumpMemGroupsWritten();
private:
    QString dspVarDataTypeToJson(int type);
    QString dspVarSegmentToJson(int segment);

    QStringList m_valueNamesList;

    struct TMemGroupWritten {
        int transcationCount = 0;
        QString memGroupName;
        //DSPDATA::dType dataType;
        QString cmdWriteString;
    };
    QList<TMemGroupWritten> m_memgroupsWritten;

    int m_transactionCount = 0;
};

typedef std::shared_ptr<TestDspInterface> TestDspInterfacePtr;

#endif // TESTDSPINTERFACE_H
