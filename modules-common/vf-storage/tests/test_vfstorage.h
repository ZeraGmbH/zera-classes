#ifndef TEST_VFSTORAGE_H
#define TEST_VFSTORAGE_H

#include "modulemanagertestrunner.h"
#include "vf_storage.h"
#include <QObject>
#include <memory>

class test_vfstorage : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void entitiesFoundMinimalSession();
    void componentsFound();
    void storeValuesBasedOnNoEntitiesInJson();
    void storeValuesBasedOnIncorrectEntitiesInJson();
    void storeValuesCorrectEntitiesStartStopLoggingDisabled();
    void storeValuesStartStopLoggingEnabledDisabled();
    void changeJsonFileWhileLogging();

private:
    void onSerialNoLicensed();
    void createModmanWithStorage();
    void startModman(QString sessionFileName);
    QEvent *generateEvent(int entityId, QString componentName, QVariant oldValue, QVariant newValue);
    QString readEntitiesAndCompoFromJsonFile(QString filePath);
    QList<QVariant> getValuesStoredOfComponent(QHash<QString, QVariant> componentHash, QString componentName);

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    std::unique_ptr<Vf_Storage> m_vfStorageEntity;
    VeinEvent::StorageSystem* m_storage;

    bool m_dataLoggerSystemInitialized = false;
};

#endif // TEST_VFSTORAGE_H
