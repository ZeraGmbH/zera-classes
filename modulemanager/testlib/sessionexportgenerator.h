#ifndef SESSIONEXPORTGENERATOR_H
#define SESSIONEXPORTGENERATOR_H

#include "modulemanagertestrunner.h"
#include "testmodulemanager.h"
#include "modulemanagerconfig.h"
#include <testdbaddsignaller.h>
#include <vf_cmd_event_handler_system.h>
#include <vl_databaselogger.h>
#include <QObject>

class SessionExportGenerator: public QObject
{
    Q_OBJECT
public:
    SessionExportGenerator(const LxdmSessionChangeParam &lxdmParam);
    ~SessionExportGenerator();

    void setDevice(QString device);
    QStringList getAvailableSessions();
    void changeSession(QString session);
    void generateDevIfaceXml(QString xmlDir);
    void generateSnapshotJsons(QString snapshotDir);
    QByteArray getVeinDump();
    QByteArray getDspMemDump();
    int getModuleConfigWriteCounts() const;
    QList<TestModuleManager::TModuleInstances> getInstanceCountsOnModulesDestroyed();

    static int getBigSessionFileCount();
private:
    void createModman(QString device);
    void createAndWriteFile(QString completeFileName, QString contents);
    void createSnapshot(QStringList contentSets, QString snapshotName);
    QString getLoggedValues(QString snapshotName);
    void clearSnapshotName();
    void fireActualValues(QString session);

    std::unique_ptr<ModuleManagerTestRunner> m_modmanTestRunner;
    ModulemanagerConfig *m_modmanConfig;
    QString m_device;
    const LxdmSessionChangeParam m_lxdmParam;
};

#endif // SESSIONEXPORTGENERATOR_H
