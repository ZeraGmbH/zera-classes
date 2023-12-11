#include "modulemanagerfortest.h"
#include <timemachineobject.h>
#include <QFile>
#include <QFileInfo>

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
    qInfo("Add %s with configfile: %s...", qPrintable(module->getVeinModuleName()), qPrintable(tmpXmlConfigFile.fileName()));
    tmpXmlConfigFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly);
    module->setConfiguration(tmpXmlConfigFile.readAll());
    TimeMachineObject::feedEventLoop();
    module->startModule();
    TimeMachineObject::feedEventLoop();
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
