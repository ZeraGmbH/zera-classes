#ifndef TEST_VEIN_DATA_COLLECTOR_H
#define TEST_VEIN_DATA_COLLECTOR_H

#include <testveinserver.h>
#include <memory>

class test_vein_data_collector : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();
    void oneChangeWithinOnePeriod();
    void twoChangesInTwoPeriods();

private:
    void setupServer();
    QJsonObject getStoredValueFromTimeStampIndex(QJsonObject newStoredValues, int timeIndex);
    QVariant getComponentValue(int entityId, QString componentName, QJsonObject storedValueWithoutTimeStamp);
    std::unique_ptr<TestVeinServer> m_server;
};

#endif // TEST_VEIN_DATA_COLLECTOR_H
