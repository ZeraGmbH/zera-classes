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
    QJsonArray dumpVariablesWritten();
private:
    QString dspVarDataTypeToJson(int type);
    QString dspVarSegmentToJson(int segment);
    QStringList m_valueNamesList;
    struct TVarsWritten {
        QString varName;
        QVector<float> dataWritten;
    };
    QList<TVarsWritten> m_valuesWritten;
};

typedef std::shared_ptr<TestDspInterface> TestDspInterfacePtr;

#endif // TESTDSPINTERFACE_H
