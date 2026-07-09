#include "test_scpi_learn_measure_objects_and_relations.h"
#include "modulemanagertestrunner.h"
#include "scpimodule.h"
#include "scpimodulenetclientblocked.h"
#include "scpitestclient.h"
#include "testdspvalues.h"
#include <timemachineobject.h>
#include <testloghelpers.h>
#include <xmldocument.h>
#include <QTest>
#include <xmldocumentcompare.h>

QTEST_MAIN(test_scpi_learn_measure_objects_and_relations)

void test_scpi_learn_measure_objects_and_relations::countMeasureRelatedObjectsCreated()
{
    // Moduleinterface initials
    ModuleManagerTestRunner testRunner(":/session-scpi-osci-for-min-measure.json");
    SCPIMODULE::cSCPIModule* scpiModule = static_cast<SCPIMODULE::cSCPIModule*>(testRunner.getModule(9999));

    constexpr int osciComponentCount = 8; // ACT_OSCI1..ACT_OSCI18
    QCOMPARE(SCPIMODULE::VeinComponentScpiMeasureSequence::getInstanceCount(), osciComponentCount);
    int noClientDelegateInstanceCount = SCPIMODULE::ScpiBaseDelegate::getInstanceCount();


    // Scpi interface copies
    SCPIMODULE::ScpiTestClient client(scpiModule, *scpiModule->getConfigData(), scpiModule->getSCPIServer()->getScpiInterface());
    client.sendScpiCmds("dev:iface?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(SCPIMODULE::VeinComponentScpiMeasureSequence::getInstanceCount(), 2 * osciComponentCount); // module + client
    // => PerVeinComponentMeasureTransaction are copied for client

    const QString xmlResponse = client.getResponsesNotSorted()[0].getStr();
    XmlDocument xmlDoc;
    xmlDoc.loadXml(xmlResponse, true);
    XmlElemIter iter = xmlDoc.root(std::make_unique<XmlElemIterStrategyTree>());
    QStringList scpiPathsOsciMeasure;
    QStringList scpiPathsNonOsciMeasure;
    while(!iter.isEnd()) {
        QDomElement element = iter.getElem();
        const QString scpiPath = element.attribute("ScpiPath");
        if (!scpiPath.isEmpty()) {
            if (scpiPath.startsWith("MEASURE"))
                scpiPathsOsciMeasure.append(scpiPath);
            else if (scpiPath.startsWith("CONFIGURE"))
                scpiPathsOsciMeasure.append(scpiPath);
            else if (scpiPath.startsWith("READ"))
                scpiPathsOsciMeasure.append(scpiPath);
            else if (scpiPath.startsWith("INIT"))
                scpiPathsOsciMeasure.append(scpiPath);
            else if (scpiPath.startsWith("FETCH"))
                scpiPathsOsciMeasure.append(scpiPath);
            else
                scpiPathsNonOsciMeasure.append(scpiPath);
        }
        iter.next();
    }

    constexpr int osciMeasureNodeCount = 50;
    QCOMPARE(scpiPathsOsciMeasure.size(), osciMeasureNodeCount);
    constexpr int nonOsciMeasureNodeCount = 33;
    QCOMPARE(scpiPathsNonOsciMeasure.size(), nonOsciMeasureNodeCount);

    QCOMPARE(noClientDelegateInstanceCount, scpiPathsNonOsciMeasure.size() + scpiPathsOsciMeasure.size());
    QCOMPARE(SCPIMODULE::ScpiBaseDelegate::getInstanceCount(), scpiPathsNonOsciMeasure.size() + 2 * scpiPathsOsciMeasure.size());
    // => Only ScpiMeasureScpiCmdNodeDelegate are copied for client
}

void test_scpi_learn_measure_objects_and_relations::multipleClientsIndependentStatusbyte()
{
    ModuleManagerTestRunner testRunner(":/session-scpi-osci-for-min-measure.json");

    ScpiModuleNetClientBlocked client1;
    ScpiModuleNetClientBlocked client2;
    QCOMPARE(client2.sendReceive("*STB?"), "+0");

    QCOMPARE(client1.sendReceive("FOO:BAR?"),"");
    QCOMPARE(client1.sendReceive("*STB?"), "+4");

    QCOMPARE(client2.sendReceive("*STB?"), "+0");

    QCOMPARE(client1.sendReceive("*CLS"),"");
    QCOMPARE(client2.sendReceive("FOO:BAR?"),"");
    QTest::qWait(300); // WTF again !!! network?

    QCOMPARE(client1.sendReceive("*STB?"), "+0");
    QCOMPARE(client2.sendReceive("*STB?"), "+4");
}

void test_scpi_learn_measure_objects_and_relations::devIfaceTwoOsci()
{
    ModuleManagerTestRunner testRunner(":/session-scpi-two-osci.json");
    SCPIMODULE::cSCPIModule* scpiModule = static_cast<SCPIMODULE::cSCPIModule*>(testRunner.getModule(9999));

    SCPIMODULE::ScpiTestClient client(scpiModule, *scpiModule->getConfigData(), scpiModule->getSCPIServer()->getScpiInterface());
    client.sendScpiCmds("dev:iface?");
    TimeMachineObject::feedEventLoop();

    const QString expectedFileName = "://dev-iface-two-osci.xml";
    QString dumped = client.getResponsesNotSorted()[0].getStr();
    QString expected = TestLogHelpers::loadFile(expectedFileName);
    XmlDocumentCompare compare;
    if (!compare.compareXml(dumped, expected))
        QVERIFY(TestLogHelpers::compareAndLogOnDiffFile(expectedFileName, dumped));
}

void test_scpi_learn_measure_objects_and_relations::sortingChangesBehaviourOnQueries()
{
    ModuleManagerTestRunner testRunner(":/session-scpi-two-osci.json");
    SCPIMODULE::cSCPIModule* scpiModule = static_cast<SCPIMODULE::cSCPIModule*>(testRunner.getModule(9999));

    SCPIMODULE::ScpiTestClient client(scpiModule, *scpiModule->getConfigData(), scpiModule->getSCPIServer()->getScpiInterface());

    client.sendScpiCmds("MEASURE:OSC1?\nMEASURE:OSC2?\n*OPC?");
    TimeMachineObject::feedEventLoop();

    // This is a major change of behaviour on sort vs. not sort!!!
    QCOMPARE(client.getResponsesSorted().size(), 0);
    QCOMPARE(client.getResponsesNotSorted().size(), 1);
}

