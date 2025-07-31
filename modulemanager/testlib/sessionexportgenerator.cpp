#include "sessionexportgenerator.h"
#include <timemachineobject.h>
#include <vcmp_remoteproceduredata.h>
#include <vf_client_rpc_invoker.h>
#include <vs_dumpjson.h>
#include <QFile>
#include <QDir>
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
}

QByteArray SessionExportGenerator::getVeinDump()
{
    return VeinStorage::DumpJson::dumpToByteArray(m_modmanTestRunner->getVeinStorageSystem()->getDb(), QList<int>(), QList<int>() << scpi_module_entity);
}

QList<TestModuleManager::TModuleInstances> SessionExportGenerator::getInstanceCountsOnModulesDestroyed()
{
    return m_modmanTestRunner->getInstanceCountsOnModulesDestroyed();
}

void SessionExportGenerator::createAndWriteFile(QString completeFileName, QString contents)
{
    QFile xmlFile(completeFileName);
    xmlFile.open(QFile::WriteOnly);
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
    VfClientRPCInvokerPtr rpc = VfClientRPCInvoker::create(vf_logger_entity);
    m_modmanTestRunner->getVfCmdEventHandlerSystemPtr()->addItem(rpc);

    QVariantMap result;
    connect(rpc.get(), &VfClientRPCInvoker::sigRPCFinished, [&result](bool ok, QUuid identifier, const QVariantMap &resultData) {
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
