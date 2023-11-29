#include "modulemanager.h"
#include "jsonsessionloader.h"
#include "modulemanagerconfig.h"
#include "customerdatasystem.h"
#include "priorityarbitrationsystem.h"
#include "zeradblogger.h"
#include "licensesystem.h"
#include "jsonloggercontentloader.h"
#include "jsonloggercontentsessionloader.h"

#include <QCoreApplication>

#include <ve_commandevent.h>
#include <vcmp_componentdata.h>
#include <vn_networksystem.h>
#include <vn_tcpsystem.h>
#include <vs_veinhash.h>
#include <vsc_scriptsystem.h>
#include <veinqml.h>
#include <veinqmlwrapper.h>

#include <vl_databaselogger.h>
#include <vl_datasource.h>
#include <vl_qmllogger.h>
#include <vl_sqlitedb.h>
#include <vf_export.h>
#include <vf_files.h>

#include <QDebug>

#include <QLoggingCategory>
#include <QStringList>
#include <QDataStream>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QRegExp>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCommandLineParser parser;
    QCommandLineOption demo("d", "Specify a value after -d", "value");
    parser.addOption(demo);
    parser.process(a);
    if(parser.isSet(demo)) {
        if(!QString(ZC_SERVICES_IP).isEmpty()) {
            fprintf(stderr, "Running in demo mode and ZC_SERIVCES_IP is set to %s. ZC_SERIVCES_IP must be empty in demo mode!\n", ZC_SERVICES_IP);
            return -ENODEV;
        }
        QString demoDeviceName = parser.value(demo);
        ModulemanagerConfig::setDemoDevice(demoDeviceName);
    }
    ModulemanagerConfig* mmConfig = ModulemanagerConfig::getInstance();
    QString configFileName = mmConfig->getConfigFileNameFull();
    qInfo() << "Moduleamanger configuration file:" << configFileName;
    if(!mmConfig->isValid()) {
        fprintf(stderr, "Error loading config file from path: %s", qPrintable(configFileName));
        return -ENOENT;
    }
    QString deviceName = mmConfig->getDeviceName();
    if(deviceName.isEmpty()) {
        fprintf(stderr, "No device name found in kernel cmdline / default config / demo param!");
        return -ENODEV;
    }
    if(!mmConfig->containsDeviceName(deviceName)) {
        fprintf(stderr, "No device found with name '%s'\n", qPrintable(deviceName));
        fprintf(stderr, "Is demo command line correct?\n");
        return -ENODEV;
    }

    qInfo() << "Loading session data for" << deviceName;
    const bool customerdataSystemEnabled = mmConfig->getCustomerDataEnabled();
    const QStringList availableSessionList = mmConfig->getAvailableSessions();
    if(availableSessionList.isEmpty()) {
        fprintf(stderr, "No sessions found for device %s", qPrintable(deviceName));
        return -ENODEV;
    }
    const QString defaultSessionFile = mmConfig->getDefaultSession();
    if(defaultSessionFile.isEmpty()) {
        fprintf(stderr, "No default session found for device %s", qPrintable(deviceName));
        return -ENODEV;
    }

    QStringList loggingFilters = QStringList() << QString("%1.debug=false").arg(VEIN_EVENT().categoryName()) <<
                                                  QString("%1.debug=false").arg(VEIN_NET_VERBOSE().categoryName()) <<
                                                  QString("%1.debug=false").arg(VEIN_NET_INTRO_VERBOSE().categoryName()) << //< Introspection logging is still enabled
                                                  QString("%1.debug=false").arg(VEIN_NET_TCP_VERBOSE().categoryName()) <<
                                                  QString("%1.debug=false").arg(VEIN_API_QML().categoryName()) <<
                                                  QString("%1.debug=false").arg(VEIN_API_QML_VERBOSE().categoryName()) <<
                                                  //                                                QString("%1.debug=false").arg(VEIN_LOGGER().categoryName()) <<
                                                  QString("%1.debug=false").arg(VEIN_STORAGE_HASH_VERBOSE().categoryName());
    QLoggingCategory::setFilterRules(loggingFilters.join("\n"));

    const VeinLogger::DBFactory sqliteFactory = [](){
        return new VeinLogger::SQLiteDB();
    };


#ifdef DEVICE_ARBITRATION
    //priority based arbitration
    PriorityArbitrationSystem *arbitrationSystem = new PriorityArbitrationSystem(&a);
    evHandler->setArbitrationSystem(arbitrationSystem);
#endif

    QString licenseUrl = QString("file://%1/license-keys").arg(OPERATOR_HOME);
    LicenseSystem *licenseSystem = new LicenseSystem({QUrl(licenseUrl)}, &a);
    ModuleManagerSetupFacade modManSetupFacade(licenseSystem, mmConfig->isDevMode(), &a);
    // setup vein modules
    VeinNet::NetworkSystem *netSystem = new VeinNet::NetworkSystem(&a);
    VeinNet::TcpSystem *tcpSystem = new VeinNet::TcpSystem(&a);
    VeinScript::ScriptSystem *scriptSystem = new VeinScript::ScriptSystem(&a);
    VeinApiQml::VeinQml *qmlSystem = new VeinApiQml::VeinQml(&a);
    ZeraDBLogger *dataLoggerSystem = new ZeraDBLogger(new VeinLogger::DataSource(modManSetupFacade.getStorageSystem(), &a), sqliteFactory, &a); //takes ownership of DataSource
    CustomerDataSystem *customerDataSystem = nullptr;
    vfExport::vf_export *exportModule=new vfExport::vf_export();

    QStringList allowedFolders{QStringLiteral(MODMAN_CUSTOMERDATA_PATH),
                               QStringLiteral(MODMAN_AUTOMOUNT_PATH),
                               QStringLiteral(MODMAN_LOGGER_LOCAL_PATH)};
    FileAccessControlPtr fileAccessController = std::make_shared<FileAccessControl>(allowedFolders);
    vfFiles::vf_files *filesModule = new vfFiles::vf_files(fileAccessController);

    //setup logger
    VeinApiQml::VeinQml::setStaticInstance(qmlSystem);
    VeinLogger::QmlLogger::setStaticLogger(dataLoggerSystem);
    VeinLogger::QmlLogger::setJsonEnvironment(MODMAN_CONTENTSET_PATH, std::make_shared<JsonLoggerContentLoader>());
    VeinLogger::QmlLogger::setJsonEnvironment(MODMAN_SESSION_PATH, std::make_shared<JsonLoggerContentSessionLoader>());

    ZeraModules::ModuleManager *modMan = new ZeraModules::ModuleManager(availableSessionList, &modManSetupFacade, &a);
    JsonSessionLoader *sessionLoader = new JsonSessionLoader(&a);

    bool demoMode = parser.isSet(demo);
    modMan->setDemo(demoMode);

    bool initQmlSystemOnce = false;
    QObject::connect(qmlSystem, &VeinApiQml::VeinQml::sigStateChanged, [&](VeinApiQml::VeinQml::ConnectionState t_state){
        if(t_state == VeinApiQml::VeinQml::ConnectionState::VQ_LOADED && initQmlSystemOnce == false)
        {
            modMan->loadScripts(scriptSystem);
            initQmlSystemOnce = true;
        }
    });

    netSystem->setOperationMode(VeinNet::NetworkSystem::VNOM_SUBSCRIPTION);
    auto errorReportFunction = [dataLoggerSystem](const QString &t_error){
        QJsonObject jsonErrorObj;

        jsonErrorObj.insert("ModuleName", "DataLogger");
        jsonErrorObj.insert("Time", QDateTime::currentDateTime().toString("yyyy/MM/dd HH:mm:ss"));
        jsonErrorObj.insert("Error", t_error);

        VeinComponent::ComponentData *cData = new VeinComponent::ComponentData();
        cData->setEntityId(0);
        cData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
        cData->setEventTarget(VeinEvent::EventData::EventTarget::ET_LOCAL);
        cData->setCommand(VeinComponent::ComponentData::Command::CCMD_SET);
        cData->setComponentName(ModuleManagerController::s_notificationMessagesComponentName);

        cData->setNewValue(jsonErrorObj);

        emit dataLoggerSystem->sigSendEvent(new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::TRANSACTION, cData));
    };
    QObject::connect(dataLoggerSystem, &VeinLogger::DatabaseLogger::sigDatabaseError, errorReportFunction);


    //do not reorder
    modManSetupFacade.addSubsystem(netSystem);
    modManSetupFacade.addSubsystem(tcpSystem);
    modManSetupFacade.addSubsystem(qmlSystem);
    modManSetupFacade.addSubsystem(scriptSystem);

    // files entity
    qInfo("Starting vf-files...");
    modManSetupFacade.addSubsystem(filesModule->getVeinEntity());
    filesModule->initOnce();
    filesModule->addMountToWatch(
                QStringLiteral("AutoMountedPaths"),
                QStringLiteral(MODMAN_AUTOMOUNT_PATH));
    filesModule->addDirToWatch(
                QStringLiteral("AvailableCustomerData"),
                QStringLiteral(MODMAN_CUSTOMERDATA_PATH),
                QStringList({"*.json", "*/*.json"}),
                QDir::NoDotAndDotDot | QDir::Files,
                false);
    filesModule->addDefaultPathComponent(
                QStringLiteral("LoggerLocalPath"),
                QStringLiteral(MODMAN_LOGGER_LOCAL_PATH),
                true);
    filesModule->addDefaultPathComponent(
                QStringLiteral("CustomerDataLocalPath"),
                QStringLiteral(MODMAN_CUSTOMERDATA_PATH),
                true);
    filesModule->addTtyWatcher("Ttys");

    //conditional systems
    bool customerDataSystemInitialized = false;
    if(customerdataSystemEnabled)
    {
        QObject::connect(licenseSystem, &LicenseSystem::sigSerialNumberInitialized, [&]() {
            if(licenseSystem->isSystemLicensed(CustomerDataSystem::s_entityName) && !customerDataSystemInitialized)
            {
                customerDataSystemInitialized = true;
                customerDataSystem = new CustomerDataSystem(&a);
                QObject::connect(customerDataSystem, &CustomerDataSystem::sigCustomerDataError, errorReportFunction);
                qDebug() << "CustomerDataSystem is enabled";
                modManSetupFacade.addSubsystem(customerDataSystem);
                customerDataSystem->initializeEntity();
            }
        });
    }
    bool dataLoggerSystemInitialized = false;
    QObject::connect(licenseSystem, &LicenseSystem::sigSerialNumberInitialized, [&](){
        if(licenseSystem->isSystemLicensed(dataLoggerSystem->entityName()))
        {
            if(!dataLoggerSystemInitialized)
            {
                dataLoggerSystemInitialized = true;
                qInfo("Starting DataLoggerSystem...");
                modManSetupFacade.addSubsystem(dataLoggerSystem);

                // exports entity
                qInfo("Starting vf-export...");
                modManSetupFacade.addSubsystem(exportModule->getVeinEntity());
                exportModule->initOnce();

                // subscribe those entitities our magic logger QML script
                // requires (see modMan->loadScripts above)
                qmlSystem->entitySubscribeById(0); //0 = mmController
                qmlSystem->entitySubscribeById(2); //2 = dataLoggerSystem
            }
        }
    });

    modMan->setLicenseSystem();

    QObject::connect(sessionLoader, &JsonSessionLoader::sigLoadModule, modMan, &ZeraModules::ModuleManager::startModule);
    QObject::connect(modMan, &ZeraModules::ModuleManager::sigSessionSwitched, sessionLoader, &JsonSessionLoader::loadSession);
    QObject::connect(modMan, &ZeraModules::ModuleManager::sigSessionSwitched, [&dataLoggerSystem]() {
        //disable logging to prevent data logging between session switching
        dataLoggerSystem->setLoggingEnabled(false);
    });

    bool modulesFound;

    qRegisterMetaTypeStreamOperators<QList<int> >("QList<int>");
    qRegisterMetaTypeStreamOperators<QList<float> >("QList<float>");
    qRegisterMetaTypeStreamOperators<QList<double> >("QList<double>");
    qRegisterMetaTypeStreamOperators<QList<QString> >("QList<QString>");
    qRegisterMetaTypeStreamOperators<QVector<QString> >("QVector<QString>");
    qRegisterMetaTypeStreamOperators<QList<QVariantMap> >("QList<QVariantMap>");



    modulesFound = modMan->loadAllAvailableModulePlugins();

    if(!modulesFound)
    {
        qCritical() << "[Zera-Module-Manager] No modules found";
        a.quit();
    }
    else
    {
        modMan->changeSessionFile(defaultSessionFile);
        tcpSystem->startServer(12000);
    }
    modMan->setupModuleManagerController();
    return a.exec();
}
