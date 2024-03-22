#include "demofactoryserviceinterfaces.h"
#include "factoryserviceinterfaces.h"
#include "modulemanager.h"
#include "modulemanagerconfig.h"
#include "licensesystem.h"

#include <QGuiApplication>

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
#include <QCommandLineParser>


static QString getDemoDeviceName(int argc, char *argv[])
{
    // QCommandLineParser freaked out without application object
    // so try oldschool
    QString demoDeviceName;
    for(int i=0; i<argc; i++) {
        if(QString(argv[i]).startsWith("-d")) {
            demoDeviceName = argv[i];
            demoDeviceName = demoDeviceName.replace("-d", "");
            demoDeviceName = demoDeviceName.simplified().remove(' ');
            break;
        }
    }
    return demoDeviceName;
}

static int exec(bool demo)
{
    if(demo)
        return QGuiApplication::exec();
    else
        return QCoreApplication::exec();
}

static void quit(bool demo)
{
    if(demo)
        QGuiApplication::quit();
    else
        QCoreApplication::quit();
}

int main(int argc, char *argv[])
{
    QString demoDeviceName = getDemoDeviceName(argc, argv);
    bool demoMode = !demoDeviceName.isEmpty();

    std::unique_ptr<QCoreApplication> app;
    if(demoMode) {
        app = std::make_unique<QGuiApplication>(argc, argv);
        ModulemanagerConfig::setDemoDevice(demoDeviceName, true);
    }
    else
        app = std::make_unique<QCoreApplication>(argc, argv);

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

    ModuleManagerSetupFacade::registerMetaTypeStreamOperators();

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
    PriorityArbitrationSystem *arbitrationSystem = new PriorityArbitrationSystem(app.get());
    evHandler->setArbitrationSystem(arbitrationSystem);
#endif

    QString licenseUrl = QString("file://%1/license-keys").arg(OPERATOR_HOME);
    LicenseSystem *licenseSystem = new LicenseSystem({QUrl(licenseUrl)}, app.get());
    ModuleManagerSetupFacade *modManSetupFacade = new ModuleManagerSetupFacade(licenseSystem, mmConfig->isDevMode(), app.get());

    AbstractFactoryServiceInterfacesPtr serviceInterfaceFactory;
    if(demoMode)
        serviceInterfaceFactory = std::make_unique<DemoFactoryServiceInterfaces>();
    else
        serviceInterfaceFactory = std::make_unique<FactoryServiceInterfaces>();
    ZeraModules::ModuleManager *modMan = new ZeraModules::ModuleManager(
        modManSetupFacade,
        serviceInterfaceFactory,
        demoMode,
        app.get());
    if(demoMode)
        modMan->startAllDemoServices(demoDeviceName);

    // setup vein modules
    VeinNet::NetworkSystem *netSystem = new VeinNet::NetworkSystem(app.get());
    VeinNet::TcpSystem *tcpSystem = new VeinNet::TcpSystem(app.get());
    VeinScript::ScriptSystem *scriptSystem = new VeinScript::ScriptSystem(app.get());
    VeinApiQml::VeinQml *qmlSystem = new VeinApiQml::VeinQml(app.get());

    QStringList allowedFolders{QStringLiteral(MODMAN_CUSTOMERDATA_PATH),
                               QStringLiteral(MODMAN_AUTOMOUNT_PATH),
                               QStringLiteral(MODMAN_LOGGER_LOCAL_PATH)};
    FileAccessControlPtr fileAccessController = std::make_shared<FileAccessControl>(allowedFolders);
    vfFiles::vf_files *filesModule = new vfFiles::vf_files(fileAccessController);

    //setup logger
    VeinApiQml::VeinQml::setStaticInstance(qmlSystem);

    bool initQmlSystemOnce = false;
    QObject::connect(qmlSystem, &VeinApiQml::VeinQml::sigStateChanged, [&](VeinApiQml::VeinQml::ConnectionState t_state){
        if(t_state == VeinApiQml::VeinQml::ConnectionState::VQ_LOADED && initQmlSystemOnce == false)
        {
            modMan->loadScripts(scriptSystem);
            initQmlSystemOnce = true;
        }
    });

    netSystem->setOperationMode(VeinNet::NetworkSystem::VNOM_SUBSCRIPTION);

    //do not reorder
    modManSetupFacade->addSubsystem(netSystem);
    modManSetupFacade->addSubsystem(tcpSystem);
    modManSetupFacade->addSubsystem(qmlSystem);
    modManSetupFacade->addSubsystem(scriptSystem);

    // files entity
    qInfo("Starting vf-files...");
    modManSetupFacade->addSubsystem(filesModule->getVeinEntity());
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

    modMan->setupConnections();

    bool modulesFound = modMan->loadAllAvailableModulePlugins();
    if(!modulesFound) {
        qCritical() << "[Zera-Module-Manager] No modules found";
        quit(demoMode);
    }
    else {
        modMan->loadDefaultSession();
        tcpSystem->startServer(12000);
    }
    return exec(demoMode);
}
