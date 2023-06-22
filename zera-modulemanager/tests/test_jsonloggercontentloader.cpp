#include "test_jsonloggercontentloader.h"
#include "jsonloggercontentloader.h"
#include <QTest>
#include <QFileInfo>

void test_jsonloggercontentloader::init()
{
}

void test_jsonloggercontentloader::cleanup()
{
}

void test_jsonloggercontentloader::noSessionSetEmptyAvailableContentSets()
{
    JsonLoggerContentLoader loader;
    QVERIFY(loader.getAvailableContentSets().isEmpty());
}

void test_jsonloggercontentloader::testSimpleAvailContentSets()
{
    JsonLoggerContentLoader loader;
    loader.setConfigFileDir(JSON_TEST_DIR);
    loader.setSession("simple-valid.json");
    QStringList cs = loader.getAvailableContentSets();
    cs.sort();
    QCOMPARE(cs.size(), 3);
    QCOMPARE(cs[0], "ZeraActualValues");
    QCOMPARE(cs[1], "ZeraDCReference");
    QCOMPARE(cs[2], "ZeraHarmonics");
}

void test_jsonloggercontentloader::testSimpleEntityComponentsNoMatch()
{
    JsonLoggerContentLoader loader;
    loader.setConfigFileDir(JSON_TEST_DIR);
    loader.setSession("simple-valid.json");
    QMap<int, QStringList> entityComponentMap = loader.getEntityComponents("foo");
    QVERIFY(entityComponentMap.isEmpty());
}

void test_jsonloggercontentloader::testSimpleEntityComponentsActual()
{
    JsonLoggerContentLoader loader;
    loader.setConfigFileDir(JSON_TEST_DIR);
    loader.setSession("simple-valid.json");
    QMap<int, QStringList> entityComponentMap = loader.getEntityComponents("ZeraActualValues");
    QCOMPARE(entityComponentMap.count(), 2);
    QCOMPARE(entityComponentMap[1040].count(), 0);
    QCOMPARE(entityComponentMap[1050].count(), 0);
}

void test_jsonloggercontentloader::testSimpleEntityComponentsHarmonics()
{
    JsonLoggerContentLoader loader;
    loader.setConfigFileDir(JSON_TEST_DIR);
    loader.setSession("simple-valid.json");
    QMap<int, QStringList> entityComponentMap = loader.getEntityComponents("ZeraHarmonics");
    QCOMPARE(entityComponentMap.count(), 2);
    QCOMPARE(entityComponentMap[1110].count(), 0);
    QCOMPARE(entityComponentMap[1060].count(), 0);
}

void test_jsonloggercontentloader::testSimpleEntityComponentsDcRef()
{
    JsonLoggerContentLoader loader;
    loader.setConfigFileDir(JSON_TEST_DIR);
    loader.setSession("simple-valid.json");
    QMap<int, QStringList> entityComponentMap = loader.getEntityComponents("ZeraDCReference");
    QCOMPARE(entityComponentMap.count(), 1);
    QCOMPARE(entityComponentMap[1050].count(), 3);
    QStringList components = entityComponentMap[1050];
    components.sort();
    QCOMPARE(components[0], "ACT_DFTPN1");
    QCOMPARE(components[1], "ACT_DFTPN2");
    QCOMPARE(components[2], "ACT_DFTPN3");
}


QTEST_MAIN(test_jsonloggercontentloader)
