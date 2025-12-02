#include "test_adj_module_lem_offset_sequences.h"
#include "adjmoduletesthelper.h"
#include <timemachineobject.h>
#include <controllerpersitentdata.h>
#include <mockserverparamgenerator.h>
#include <QTest>

QTEST_MAIN(test_adj_module_lem_offset_sequences)

void test_adj_module_lem_offset_sequences::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    setupServers();
    QCOMPARE(setRangeGroupingOff(), "+0");
    QCOMPARE(setAutoRangeOff(), "+0");
}

void test_adj_module_lem_offset_sequences::cleanupTestCase()
{
    m_scpiClient.reset();
    TimeMachineObject::feedEventLoop();
    m_testRunner.reset();
    TimeMachineObject::feedEventLoop();
    ControllerPersitentData::cleanupPersitentData();
}

void test_adj_module_lem_offset_sequences::init()
{
    setLogFileName(QTest::currentTestFunction(), QTest::currentDataTag());
}

void test_adj_module_lem_offset_sequences::cleanup()
{
    m_testRunner->removeAllHotplugDevices();
}

void test_adj_module_lem_offset_sequences::adjOffsetActRefZero()
{
    insertClamps(cClamp::CL1400VDC, cClamp::CL1200ADC1400VDC);
    QCOMPARE(setRange("UL1", "C1400V"), "+0");
    AdjModuleTestHelper::setAllValuesSymmetricDc(*m_testRunner, 0.0, 0.0);

    QCOMPARE(adjInit("UL1", "C1400V"), "+0");
    QCOMPARE(adjSendOffset("UL1", "C1400V", "0.0"), "+0");
    QCOMPARE(adjCompute(), "+0");

    QStringList nodes = queryAdjNodesOrCoeff("UL1", "C1400V", NODE);

    QStringList coeffs = queryAdjNodesOrCoeff("UL1", "C1400V", COEFFICIENT);
    QCOMPARE(coeffs.size(), 4);
    // we adjusted with reference value 0.0 => all coeefs 0
    QCOMPARE(coeffs[0], "0");
    QCOMPARE(coeffs[1], "0");
    QCOMPARE(coeffs[2], "0");
    QCOMPARE(coeffs[3], "0");
}

void test_adj_module_lem_offset_sequences::adjOffsActRefNonZeroSame()
{
    insertClamps(cClamp::CL1400VDC, cClamp::CL1200ADC1400VDC);
    QCOMPARE(setRange("UL1", "C1400V"), "+0");
    AdjModuleTestHelper::setAllValuesSymmetricDc(*m_testRunner, 1.25, 0.0);

    QCOMPARE(adjInit("UL1", "C1400V"), "+0");
    QCOMPARE(adjSendOffset("UL1", "C1400V", "1.25"), "+0");
    QCOMPARE(adjCompute(), "+0");

    QStringList coeffs = queryAdjNodesOrCoeff("UL1", "C1400V", COEFFICIENT);
    QCOMPARE(coeffs.size(), 4);
    QCOMPARE(coeffs[0], "0");
    QCOMPARE(coeffs[1], "0");
    QCOMPARE(coeffs[2], "0");
    QCOMPARE(coeffs[3], "0");
}

void test_adj_module_lem_offset_sequences::adjOffsActNonZeroRefZero()
{
    insertClamps(cClamp::CL1400VDC, cClamp::CL1200ADC1400VDC);
    QCOMPARE(setRange("UL1", "C1400V"), "+0");
    AdjModuleTestHelper::setAllValuesSymmetricDc(*m_testRunner, 1.0, 0.0);

    QCOMPARE(adjInit("UL1", "C1400V"), "+0");
    QCOMPARE(adjSendOffset("UL1", "C1400V", "0.0"), "+0");
    QCOMPARE(adjCompute(), "+0");

    QStringList nodes = queryAdjNodesOrCoeff("UL1", "C1400V", NODE);
    QList<NodeVal> nodeValList = decodeNodes(nodes);
    constexpr double expectedOffset = -2832.860000;
    QCOMPARE(nodeValList[0].sampleOffset, expectedOffset);
    QCOMPARE(nodeValList[0].takenAt, 0.0);
    QCOMPARE(nodeValList[1].sampleOffset, expectedOffset);
    QCOMPARE(nodeValList[1].takenAt, 0.0);
    QCOMPARE(nodeValList[2].sampleOffset, expectedOffset);
    QCOMPARE(nodeValList[2].takenAt, 0.0);
    QCOMPARE(nodeValList[3].sampleOffset, expectedOffset);
    QCOMPARE(nodeValList[3].takenAt, 0.0);

    QStringList coeffs = queryAdjNodesOrCoeff("UL1", "C1400V", COEFFICIENT);
    QCOMPARE(coeffs.size(), 4);
    QCOMPARE(coeffs[0], QString::number(expectedOffset));
    QCOMPARE(coeffs[1], "0");
    QCOMPARE(coeffs[2], "0");
    QCOMPARE(coeffs[3], "0");
}

void test_adj_module_lem_offset_sequences::writeAdjValuesAfterClampInserted()
{
    insertClamps(cClamp::CL1400VDC, cClamp::CL1200ADC1400VDC);
}

void test_adj_module_lem_offset_sequences::allClampsPermissions_data()
{
    QTest::addColumn<int>("clampType");
    QTest::addColumn<QByteArray>("voltageRange");
    QTest::addColumn<QByteArray>("currentRange");

    const QList<cClamp::ClampTypes> clampTypes =
        QList<cClamp::ClampTypes>() << cClamp::CL1400VDC << cClamp::CL8ADC1400VDC << cClamp::CL1200ADC1400VDC << // new
                                                 cClamp::CL200ADC1000VDC << cClamp::CL1000VDC; // old

    for (const cClamp::ClampTypes &clampType : clampTypes) {
        QList<AbstractMockAllServices::clampParam> clampParams;
        clampParams.append({"IAUX", clampType});
        m_testRunner->addClamps(clampParams);

        const QByteArrayList allVoltageRanges = m_scpiClient->sendReceive("SENSE:RNG1:UAUX:RANGE:CATALOG?").split(';');
        QByteArrayList clampVoltageRanges;
        for(const QByteArray &voltageRange : allVoltageRanges) {
            if(voltageRange.startsWith('C'))
                clampVoltageRanges.append(voltageRange);
        }
        const QByteArrayList allCurrentRanges = m_scpiClient->sendReceive("SENSE:RNG1:IAUX:RANGE:CATALOG?").split(';');
        QByteArrayList clampCurrentRanges;
        for(const QByteArray &currentRange : allCurrentRanges) {
            if(currentRange.startsWith('C'))
                clampCurrentRanges.append(currentRange);
        }

        for(const QByteArray &voltageRange : clampVoltageRanges)
            QTest::newRow(qPrintable(cClamp::getClampTypeName(clampType) + "_" + voltageRange)) <<
                int(clampType) << voltageRange << QByteArray();
        for(const QByteArray &currentRange : clampCurrentRanges)
            QTest::newRow(qPrintable(cClamp::getClampTypeName(clampType) + "_"  + currentRange)) <<
                int(clampType) << QByteArray() << currentRange;
        m_testRunner->removeAllHotplugDevices();
    }
}

void test_adj_module_lem_offset_sequences::allClampsPermissions()
{
    QFETCH(int, clampType);
    QFETCH(QByteArray, voltageRange);
    QFETCH(QByteArray, currentRange);
    QByteArray channel = "UAUX";
    QByteArray range = voltageRange;
    if(!currentRange.isEmpty()) {
        channel = "IAUX";
        range = currentRange;
    }

    QList<AbstractMockAllServices::clampParam> clampParams;
    clampParams.append({"IAUX", cClamp::ClampTypes(clampType)});
    m_testRunner->addClamps(clampParams);

    setRange(channel, range);
    QCOMPARE(adjInit(channel, range), "+0");
    QCOMPARE(adjSendOffset(channel, range, "0.0"), "+0");
    QCOMPARE(adjCompute(), "+0");
}

void test_adj_module_lem_offset_sequences::internalNoPermission()
{
    QByteArray channel = "UAUX";
    QByteArray range = "250V";
    QCOMPARE(setRange(channel, range), "+0");

    // INIT is handled on adjustmentmodule only / no command send => always allowed :(
    QCOMPARE(adjInit(channel, range), "+0");

    QCOMPARE(adjSendOffset(channel, range, "0.0"), "+4");
    QCOMPARE(clearScpiStatus(), "+0");

    QCOMPARE(adjCompute(), "+4");
    QCOMPARE(clearScpiStatus(), "+0");
}

void test_adj_module_lem_offset_sequences::setupServers()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>("mt310s2-dc-session.json");
    m_scpiClient = std::make_unique<ScpiModuleClientBlocked>();
}

void test_adj_module_lem_offset_sequences::insertClamps(cClamp::ClampTypes clampPhase1To3,
                                                        cClamp::ClampTypes clampPhaseAux)
{
    QList<AbstractMockAllServices::clampParam> clampParams;
    clampParams.append({"IL1", clampPhase1To3});
    clampParams.append({"IL2", clampPhase1To3});
    clampParams.append({"IL3", clampPhase1To3});
    clampParams.append({"IAUX", clampPhaseAux});
    m_testRunner->addClamps(clampParams);
}

QString test_adj_module_lem_offset_sequences::clearScpiStatus()
{
    const QByteArray send = "*CLS|*STB?";
    return m_scpiClient->sendReceive(send);
}

QString test_adj_module_lem_offset_sequences::setAutoRangeOff()
{
    const QByteArray send = "CONFIGURATION:RNG1:RNGAUTO 0;|*stb?";
    return m_scpiClient->sendReceive(send);
}

QString test_adj_module_lem_offset_sequences::setRangeGroupingOff()
{
    const QByteArray send = "CONFIGURATION:RNG1:GROUPING 0;|*stb?";
    return m_scpiClient->sendReceive(send);
}

QString test_adj_module_lem_offset_sequences::setRange(const QString &channelAlias, const QString &rangeName)
{
    const QByteArray send = QString("SENSE:RNG1:%1:RANGE %2;|*stb?").arg(channelAlias, rangeName).toLocal8Bit();
    return m_scpiClient->sendReceive(send);
}

bool test_adj_module_lem_offset_sequences::setAllRanges(const QString &voltageRangeName, const QString &currrentRangeName)
{
    return setRange("UL1", voltageRangeName) == "+0" &&
           setRange("UL2", voltageRangeName) == "+0" &&
           setRange("UL3", voltageRangeName) == "+0" &&
           setRange("UAUX", voltageRangeName) == "+0" &&
           setRange("IAUX", currrentRangeName) == "+0";
}

QString test_adj_module_lem_offset_sequences::adjInit(const QString &channelAlias, const QString &rangeName)
{
    const QByteArray send = QString("CALC:ADJ1:INIT %1,%2;|*stb?").arg(channelAlias, rangeName).toLocal8Bit();
    return m_scpiClient->sendReceive(send);
}

QString test_adj_module_lem_offset_sequences::adjSendOffset(const QString &channelAlias, const QString &rangeName,
                                                         const QString &offset)
{
    const QByteArray send = QString("CALC:ADJ1:OFFS %1,%2,%3;|*stb?").arg(channelAlias, rangeName, offset).toLocal8Bit();
    return m_scpiClient->sendReceive(send);
}

QString test_adj_module_lem_offset_sequences::adjCompute()
{
    const QByteArray send = "CALC:ADJ1:COMP 1;|*stb?";
    return m_scpiClient->sendReceive(send);
}

QStringList test_adj_module_lem_offset_sequences::queryAdjNodesOrCoeff(const QString &channelAlias,
                                                                       const QString &rangeName,
                                                                       CoeffOrNode coeffOrNode)
{
    cSenseSettingsPtr senseSettings = getMt310s2dSenseSettings();
    SenseSystem::cChannelSettings* channelSetting = senseSettings->findChannelSettingByAlias1(channelAlias);
    const QString channelMName = channelSetting->m_nameMx;
    const QString valueType = coeffOrNode == NODE ? "NODE" : "COEFFICIENT";
    QStringList offsetCoeffs;
    for(int coeffNo=0; coeffNo<4; ++coeffNo) {
        const QByteArray send = QString("CALC:ADJ1:SEND? 6307,SENSE:%1:%2:CORRECTION:OFFS:%3:%4?;").
                                arg(channelMName, rangeName, valueType).arg(coeffNo).
                                toLocal8Bit();
        QString receive = m_scpiClient->sendReceive(send);
        QStringList semicolonSepList = receive.split(";", Qt::SkipEmptyParts);
        offsetCoeffs.append(semicolonSepList.join(";"));
    }
    return offsetCoeffs;
}

QList<test_adj_module_lem_offset_sequences::NodeVal> test_adj_module_lem_offset_sequences::decodeNodes(const QStringList nodes)
{
    QList<NodeVal> decodedNodes;
    for (int nodeNo=0; nodeNo<4; ++nodeNo) {
        QStringList nodeParts = nodes[nodeNo].split(";");
        decodedNodes.append({nodeParts[0].toDouble(), nodeParts[1].toDouble()});
    }
    return decodedNodes;
}

cSenseSettingsPtr test_adj_module_lem_offset_sequences::getMt310s2dSenseSettings()
{
    ServerParams params = MockServerParamGenerator::createParams("mt310s2d");
    SettingsContainerPtr serviceSettings = std::make_unique<SettingsContainer>(params);
    return serviceSettings->getSenseSettings();
}

void test_adj_module_lem_offset_sequences::setLogFileName(const QString &currentTestFunction, const QString &currentDataTag)
{
    QString fileName = "/tmp/test_adj_module_lem_offset_sequences/" + currentTestFunction;
    if(!currentDataTag.isEmpty())
        fileName += "_" + currentDataTag;
    m_scpiClient->setLogFile(fileName + ".log");
}
