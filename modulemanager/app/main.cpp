#include "demofactoryserviceinterfaces.h"
#include "demomodulemanager.h"
#include "factoryserviceinterfaces.h"
#include "modulemanager.h"
#include "modulemanagerconfig.h"
#include "customerdatasystem.h"
#include "licensesystem.h"

#include <QGuiApplication>

#include <contentsetsotherfromcontentsetsconfig.h>
#include <contentsetszeraallfrommodmansessions.h>
#include <ve_commandevent.h>
#include <vcmp_componentdata.h>
#include <vn_networksystem.h>
#include <vn_tcpsystem.h>
#include <vs_storageeventsystem.h>
#include <vl_databaselogger.h>
#include <loggercontentsetconfig.h>
#include <vl_sqlitedb.h>
#include <vf_export.h>
#include <vf_files.h>
#include <tcpnetworkfactory.h>
#include "mocklxdmsessionchangeparamgenerator.h"
#include <proxy.h>

#include <QDebug>
#include <QCommandLineParser>
#include <memory>

static bool serverStarted = false;
static VeinNet::NetworkSystem *netSystem = nullptr;
static VeinNet::TcpSystem *tcpSystem = nullptr;
static std::unique_ptr<ModuleManagerSetupFacade> modManSetupFacade;
static VeinStorage::AbstractComponentPtr entititesComponent;

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

static void startNetwork(QObject *parent)
{
    if(!serverStarted) {
        netSystem = new VeinNet::NetworkSystem(parent);
        netSystem->setOperationMode(VeinNet::NetworkSystem::VNOM_SUBSCRIPTION);
        tcpSystem = new VeinNet::TcpSystem(VeinTcp::TcpNetworkFactory::create(), parent);
        //do not reorder
        modManSetupFacade->addSubsystem(netSystem);
        modManSetupFacade->addSubsystem(tcpSystem);
        tcpSystem->startServer(12000);
        serverStarted = true;
    }
}

static VeinTcp::AbstractTcpNetworkFactoryPtr serviceNetworkFactory;
static VeinTcp::AbstractTcpNetworkFactoryPtr getServiceNetworkFactory() {
    if (serviceNetworkFactory == nullptr)
        serviceNetworkFactory = VeinTcp::TcpNetworkFactory::create();
    return serviceNetworkFactory;
}

static ZeraModules::ModuleManager *createModMan(bool demoMode,
                                                const QString& demoDeviceName,
                                                QObject* parent)
{
    if(demoMode) {
        DemoModuleManager* demoModMan = new DemoModuleManager(
            modManSetupFacade.get(),
            std::make_unique<DemoFactoryServiceInterfaces>(),
            getServiceNetworkFactory(),
            parent);
        demoModMan->startAllDemoServices(demoDeviceName);
        return demoModMan;
    }
    else {
        return new ZeraModules::ModuleManager(
            modManSetupFacade.get(),
            std::make_unique<FactoryServiceInterfaces>(),
            getServiceNetworkFactory(),
            false,
            parent);
    }
}

static ZeraModules::ModuleManager *modMan = nullptr;
static std::unique_ptr<QCoreApplication> app;
Zera::ProxyClientPtr m_dummyPcbClient;
static bool startModmanRequired = false;

void startModman()
{
    modMan->loadDefaultSession();
    VeinStorage::AbstractEventSystem *storage = modManSetupFacade->getStorageSystem();
    entititesComponent = storage->getDb()->getFutureComponent(0, "Entities");
    QObject::connect(entititesComponent.get(), &VeinStorage::AbstractComponent::sigValueChange, entititesComponent.get(), [&] (QVariant newValue) {
        if(newValue.isValid())
            startNetwork(app.get());
    });
}

static void openDummyPcbConnectionToStartServiceEarly()
{
    // Due to atmel updater conflicting with mt310s2d.service, mt310s2d.service
    // is started by socket connection. To avoid statusmodule waiting for
    // mt310s2d.service to start completely, start it early
    ModulemanagerConfig* mmConfig = ModulemanagerConfig::getInstance();
    m_dummyPcbClient = Zera::Proxy::getInstance()->getConnectionSmart(
                           mmConfig->getPcbConnectionInfo(),
                           getServiceNetworkFactory()),
    QObject::connect(m_dummyPcbClient.get(), &Zera::ProxyClient::connected, [&]() {
        if (startModmanRequired)
            startModman();
    });
    Zera::Proxy::getInstance()->startConnectionSmart(m_dummyPcbClient);
}

int main(int argc, char *argv[])
{
    QString demoDeviceName = getDemoDeviceName(argc, argv);
    bool demoMode = !demoDeviceName.isEmpty();

    if(demoMode) {
        app = std::make_unique<QGuiApplication>(argc, argv);
        ModulemanagerConfig::setDemoDevice(demoDeviceName);
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

    qInfo("Register MetaTypeStreamOperators...");
    ModuleManagerSetupFacade::registerMetaTypeStreamOperators();

    QStringList loggingFilters = QStringList() << QString("%1.debug=false").arg(VEIN_EVENT().categoryName()) <<
                                                  QString("%1.debug=false").arg(VEIN_NET_VERBOSE().categoryName()) <<
                                                  QString("%1.debug=false").arg(VEIN_NET_INTRO_VERBOSE().categoryName()) << //< Introspection logging is still enabled
                                                  QString("%1.debug=false").arg(VEIN_NET_TCP_VERBOSE().categoryName());
    qInfo("Set logging filter rules...");
    QLoggingCategory::setFilterRules(loggingFilters.join("\n"));

    const VeinLogger::DBFactory sqliteFactory = [](){
        return std::make_shared<VeinLogger::SQLiteDB>();
    };

    qInfo("Create license system...");
    QString licenseUrl = QString("file://%1/license-keys").arg(OPERATOR_HOME);
    LicenseSystem *licenseSystem = new LicenseSystem({QUrl(licenseUrl)}, app.get());
    qInfo("Create modman objects...");
    if(!demoMode)
        modManSetupFacade = std::make_unique<ModuleManagerSetupFacade>(licenseSystem, demoMode);
    else
        modManSetupFacade = std::make_unique<ModuleManagerSetupFacade>(licenseSystem, demoMode,
            MockLxdmSessionChangeParamGenerator::generateDemoSessionChanger());
    modMan = createModMan(demoMode, demoDeviceName, app.get());
    openDummyPcbConnectionToStartServiceEarly();

    // setup vein modules
    VeinLogger::DatabaseLogger *dataLoggerSystem = new VeinLogger::DatabaseLogger(
        modManSetupFacade->getStorageSystem(),
        sqliteFactory,
        app.get(),
        QList<int>()
            << 0    /* SYSTEM */
            << 1150 /* STATUS */);
    CustomerDataSystem *customerDataSystem = nullptr;
    vfExport::vf_export *exportModule=new vfExport::vf_export();

    QStringList allowedFolders{QStringLiteral(MODMAN_CUSTOMERDATA_PATH),
                               QStringLiteral(MODMAN_AUTOMOUNT_PATH),
                               QStringLiteral(MODMAN_LOGGER_LOCAL_PATH)};
    FileAccessControlPtr fileAccessController = std::make_shared<FileAccessControl>(allowedFolders);
    vfFiles::vf_files *filesModule = new vfFiles::vf_files(fileAccessController);

    //setup logger
    VeinLogger::LoggerContentSetConfig::setJsonEnvironment(MODMAN_CONTENTSET_PATH, std::make_shared<ContentSetsOtherFromContentSetsConfig>());
    VeinLogger::LoggerContentSetConfig::setJsonEnvironment(MODMAN_SESSION_PATH, std::make_shared<ContentSetsZeraAllFromModmanSessions>());

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

    //conditional systems
    bool customerDataSystemInitialized = false;
    if(customerdataSystemEnabled)
    {
        QObject::connect(licenseSystem, &LicenseSystem::sigSerialNumberInitialized, [&]() {
            if(licenseSystem->isSystemLicensed(CustomerDataSystem::s_entityName) && !customerDataSystemInitialized)
            {
                customerDataSystemInitialized = true;
                qInfo("Starting CustomerDataSystem");
                customerDataSystem = new CustomerDataSystem(MODMAN_CUSTOMERDATA_PATH, app.get());
                modManSetupFacade->addSubsystem(customerDataSystem);
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
                modManSetupFacade->addSubsystem(dataLoggerSystem);

                // exports entity
                qInfo("Starting vf-export...");
                modManSetupFacade->addSubsystem(exportModule->getVeinEntity());
                exportModule->initOnce();
            }
        }
    });

    modMan->setupConnections();

    bool modulesFound = modMan->loadAllAvailableModulePlugins();
    if(!modulesFound) {
        qCritical() << "[Zera-Module-Manager] No modules found";
        quit(demoMode);
    }
    else
        startModmanRequired = true;

    return exec(demoMode);
}
