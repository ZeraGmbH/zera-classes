#include "modulemanagerfortest.h"
#include <QFile>
#include <QFileInfo>
#include <QCoreApplication>
#include <QAbstractEventDispatcher>

ModuleManagerForTest::ModuleManagerForTest()
{
    addSystem(&m_storageSystem);
}

void ModuleManagerForTest::addModule(cBaseModule *module, QString configFileFullPath)
{
    connect(module, &ZeraModules::VirtualModule::addEventSystem, this, [&] (VeinEvent::EventSystem *eventSystem) {
        addSystem(eventSystem);
    });

    QFileInfo fileInfo(configFileFullPath);
    QFile tmpXmlConfigFile(fileInfo.absoluteFilePath());
    qInfo("Add %s with configfile: %s...", qPrintable(module->getModuleName()), qPrintable(tmpXmlConfigFile.fileName()));
    tmpXmlConfigFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly);
    module->setConfiguration(tmpXmlConfigFile.readAll());
    feedEventLoop();
    module->startModule();
    feedEventLoop();
}

VeinStorage::VeinHash *ModuleManagerForTest::getStorageSystem()
{
    return &m_storageSystem;
}

void ModuleManagerForTest::addSystem(VeinEvent::EventSystem *subsystem)
{
    if(!m_addedSubsystems.contains(subsystem)) {
        // On startup modules can emit addEventSystem multiple times for some
        // systems. So add them once only (see ModuleEventHandler::addSystem -
        // it does same)
        m_addedSubsystems.insert(subsystem);
        m_eventHandler.addSubsystem(subsystem);
    }
}

void ModuleManagerForTest::feedEventLoop()
{
    while(QCoreApplication::eventDispatcher()->processEvents(QEventLoop::AllEvents));
}
