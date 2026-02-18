#include "sessionexportgenerator.h"
#include "testdspvalues.h"
#include <timemachineobject.h>
#include <vcmp_remoteproceduredata.h>
#include <vf_client_rpc_invoker.h>
#include <vf_rpc_invoker.h>
#include <vs_dumpjson.h>
#include <testloghelpers.h>
#include <QFile>
#include <QDir>
#include <QJsonObject>
#include <memory>

constexpr int system_entity = 0;
constexpr int scpi_module_entity = 9999;
constexpr int vf_logger_entity = 2;

SessionExportGenerator::SessionExportGenerator(const LxdmSessionChangeParam &lxdmParam) :
    m_lxdmParam(lxdmParam)
{
    ModuleManagerSetupFacade::registerMetaTypeStreamOperators();
    ModulemanagerConfig::setDemoDevice("mt310s2");
    m_modmanConfig = ModulemanagerConfig::getInstance();
}

SessionExportGenerator::~SessionExportGenerator()
{
}

void SessionExportGenerator::createModman(QString device)
{
    qInfo("Create modman for device: %s\n", qPrintable(device));
    ModulemanagerConfig::setDemoDevice(device);
    m_modmanTestRunner = std::make_unique<ModuleManagerTestRunner>("", false, device, m_lxdmParam, true);

    QFile::remove("/tmp/test.db");
    m_modmanTestRunner->setVfComponent(vf_logger_entity, "DatabaseFile", "/tmp/test.db");
    m_modmanTestRunner->setVfComponent(vf_logger_entity, "sessionName", device + "TestSession");
}

void SessionExportGenerator::setDevice(QString device)
{
    if(m_device != device) {
        m_modmanTestRunner.reset();
        createModman(device);
        m_device = device;
    }
}

QStringList SessionExportGenerator::getAvailableSessions()
{
    return m_modmanConfig->getAvailableSessions();
}

void SessionExportGenerator::changeSession(QString session)
{
    m_modmanTestRunner->start(session);
    fireActualValues(session);
}

void SessionExportGenerator::generateDevIfaceXml(QString xmlDir)
{
    QString scpiIface = m_modmanTestRunner->getVfComponent(scpi_module_entity, "ACT_DEV_IFACE").toString();
    QString currentSession = m_modmanTestRunner->getVfComponent(system_entity, "Session").toString();
    QString xmlFileName(xmlDir + currentSession);
    xmlFileName.replace("json", "xml");
    createAndWriteFile(xmlFileName, scpiIface);
}

void SessionExportGenerator::generateSnapshotJsons(QString snapshotDir)
{
    QString currentSession = m_modmanTestRunner->getVfComponent(system_entity, "Session").toString();
    const QStringList availableContentSets = m_modmanTestRunner->getVfComponent(vf_logger_entity, "availableContentSets").toStringList();
    for(const QString &contentSet: availableContentSets) {
        QString snapshotName = currentSession.replace(".json", "") + '-' + contentSet;
        createSnapshot(QStringList() << contentSet, snapshotName);
        QString snapshotContents = getLoggedValues(snapshotName);
        createAndWriteFile(QDir::cleanPath(snapshotDir + "/" + snapshotName + ".json"), snapshotContents);
        clearSnapshotName(); //so that vein-dumps are always clean and independent from snapshots/transaction details
    }
    for (int i=0; i<getBigSessionFileCount(); ++i) {
        QFile file(QDir::cleanPath(snapshotDir + "/" + "mt310s2-meas-session-ZeraAll.json"));
        QString num = QString("0%1").arg(i+1).right(2);
        QString fileName = QString("big-%1.json").arg(num);
        file.copy(QDir::cleanPath(snapshotDir + "/" + fileName));
    }
}

QByteArray SessionExportGenerator::getVeinDump()
{
    return VeinStorage::DumpJson::dumpToByteArray(
        m_modmanTestRunner->getVeinStorageDb(),
        QList<int>(),
        QList<int>() << scpi_module_entity,
        true);
}

QByteArray SessionExportGenerator::getDspMemDump()
{
    ZDspServer *dspServer = m_modmanTestRunner->getDspServer();
    QJsonObject json;
    json.insert("ProgMemCyclicAvailable", dspServer->getProgMemCyclicAvailable());
    json.insert("ProgMemCyclicFree", dspServer->getProgMemCyclicAvailable()-dspServer->getProgMemCyclicOccupied());
    json.insert("ProgMemInterruptAvailable", dspServer->getProgMemInterruptAvailable());
    json.insert("ProgMemInterruptFree", dspServer->getProgMemInterruptAvailable() - dspServer->getProgMemInterruptOccupied());
    json.insert("UserMemAvailable", dspServer->getUserMemAvailable());
    json.insert("UserMemFree", dspServer->getUserMemAvailable() - dspServer->getUserMemOccupied());
    return TestLogHelpers::dump(json);
}

QByteArray SessionExportGenerator::getDspVarDump()
{
    QMap<int, QList<TestDspInterfacePtr>> dspInterfaces = m_modmanTestRunner->getAllDspInterfaces();
    QJsonObject json;
    for(auto entityIter=dspInterfaces.cbegin(); entityIter!=dspInterfaces.cend(); entityIter++) {
        int entityId = entityIter.key();
        QString entityIdStr = QString("%1").arg(entityId);
        const QList<TestDspInterfacePtr> &entityInterfaces = entityIter.value();
        for (const TestDspInterfacePtr &interface : entityInterfaces) {
            QJsonObject dump = interface->dumpAll();
            json.insert(entityIdStr, dump);
        }
    }
    return TestLogHelpers::dump(json);
}

QByteArray SessionExportGenerator::getSecUnitDump()
{
    cSEC1000dServer *secServer = m_modmanTestRunner->getSecServer();
    QJsonObject json;
    json.insert("EcUnitsAvailable", secServer->getEcUnitsAvailable());
    json.insert("EcUnitsFree", secServer->getEcUnitsAvailable()-secServer->getEcUnitsOccupied());
    return TestLogHelpers::dump(json);
}

int SessionExportGenerator::getModuleConfigWriteCounts() const
{
    return m_modmanTestRunner->getModuleConfigWriteCounts();
}

QList<TestModuleManager::TModuleInstances> SessionExportGenerator::getInstanceCountsOnModulesDestroyed()
{
    return m_modmanTestRunner->getInstanceCountsOnModulesDestroyed();
}

int SessionExportGenerator::getBigSessionFileCount()
{
    return 20;
}

void SessionExportGenerator::createAndWriteFile(QString completeFileName, QString contents)
{
    QFile xmlFile(completeFileName);
    if (!xmlFile.open(QFile::WriteOnly))
        qCritical("Could not open %s", qPrintable(completeFileName));
    xmlFile.write(contents.toUtf8());
}

void SessionExportGenerator::createSnapshot(QStringList contentSets, QString snapshotName)
{
    m_modmanTestRunner->setVfComponent(vf_logger_entity, "currentContentSets", contentSets);
    m_modmanTestRunner->setVfComponent(vf_logger_entity, "transactionName", snapshotName);
    m_modmanTestRunner->setVfComponent(vf_logger_entity, "LoggingEnabled", true);
    m_modmanTestRunner->setVfComponent(vf_logger_entity, "LoggingEnabled", false);
}

QString SessionExportGenerator::getLoggedValues(QString snapshotName)
{
    VfRPCInvokerPtr rpc= VfRPCInvoker::create(vf_logger_entity, std::make_unique<VfClientRPCInvoker>());
    m_modmanTestRunner->getVfCmdEventHandlerSystemPtr()->addItem(rpc);

    QVariantMap result;
    connect(rpc.get(), &VfRPCInvoker::sigRPCFinished, [&result](bool ok, const QVariantMap &resultData) {
        result = resultData;
    });

    QVariantMap rpcParams;
    rpcParams.insert("p_transaction", snapshotName);
    rpc->invokeRPC("RPC_displayActualValues", rpcParams);
    TimeMachineObject::feedEventLoop();
    m_modmanTestRunner->getVfCmdEventHandlerSystemPtr()->removeItem(rpc);

    if(result.contains(VeinComponent::RemoteProcedureData::s_returnString)) {
        QJsonObject returnedJson = result[VeinComponent::RemoteProcedureData::s_returnString].toJsonObject();
        QJsonDocument Doc(returnedJson);
        QByteArray ba = Doc.toJson();
        return QString(ba);
    }
    return QString();
}

void SessionExportGenerator::clearSnapshotName()
{
    m_modmanTestRunner->setVfComponent(vf_logger_entity, "transactionName", "");
}

void SessionExportGenerator::fireActualValues(QString session)
{
    constexpr double testvoltage = 120;
    constexpr double testcurrent = 10;
    constexpr double testangle = 0;
    constexpr double testfrequency = 50;

    TestDspValues dspValues(m_modmanTestRunner->getDspInterface(INJECT_DFT)->getValueList());
    if(session.contains("meas") || session.contains("perphase") || session.contains("ced")) {
        dspValues.setAllValuesSymmetric(testvoltage, testcurrent, testangle, testfrequency);
        dspValues.fireAllActualValues(
            m_modmanTestRunner->getDspInterface(INJECT_DFT),
            m_modmanTestRunner->getDspInterface(INJECT_FFT),
            m_modmanTestRunner->getDspInterface(INJECT_RANGE_PROGRAM), // Range is for frequency only
            m_modmanTestRunner->getDspInterface(INJECT_RMS));
    }
    else if(session.contains("ac")) {
        dspValues.setAllValuesSymmetricAc(testvoltage, testcurrent, testangle, testfrequency);
        dspValues.fireAllActualValues(
            m_modmanTestRunner->getDspInterface(INJECT_DFT),
            m_modmanTestRunner->getDspInterface(INJECT_FFT),
            m_modmanTestRunner->getDspInterface(INJECT_RANGE_PROGRAM),
            m_modmanTestRunner->getDspInterface(INJECT_RMS));
    }
    else if(session.contains("dc")) {
        dspValues.setAllValuesSymmetricDc(testvoltage, testcurrent);
        dspValues.fireAllActualValues(
            m_modmanTestRunner->getDspInterface(INJECT_DFT),
            m_modmanTestRunner->getDspInterface(INJECT_FFT),
            m_modmanTestRunner->getDspInterface(INJECT_RANGE_PROGRAM),
            m_modmanTestRunner->getDspInterface(INJECT_RMS));
    }
}
