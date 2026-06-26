#ifndef TESTDSPINTERFACE_H
#define TESTDSPINTERFACE_H

#include <mockdspinterface.h>
#include <QJsonObject>
#include <QJsonArray>

class TestDspInterface : public MockDspInterface
{
    Q_OBJECT
public:
    explicit TestDspInterface(const QStringList &valueNamesList, int entityId);
    void setClientSmart(Zera::ProxyClientPtr client) override;
    quint32 dspMemoryWrite(DspVarGroupClientInterface* varGroup) override;

    QStringList getValueList();
    QJsonObject dumpAll(bool dumpVarWrite = false);
    QJsonObject dumpMemoryGroups();
    QJsonObject dumpVarList(QJsonObject inData);
    QStringList dumpCycListItem();
    QJsonObject dumpVariablesWritten();
private slots:
    void onConnect();
private:
    QString dspVarDataTypeToJson(int type);
    QString dspVarSegmentToJson(int segment);
    bool checkDspVar(DspVarClientInterface* dspVar);

    QStringList m_valueNamesList;
    struct TVarsWritten {
        int transcationCount;
        QString groupVarName;
        double dataWritten; // enough precision for uint (32 bit)
    };
    QList<TVarsWritten> m_valuesWritten;
    int m_transactionCount = 0;
};

typedef std::shared_ptr<TestDspInterface> TestDspInterfacePtr;

#endif // TESTDSPINTERFACE_H
