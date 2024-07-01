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
    void fireActualValuesAfterDelayWhileLogging();

private:
    void onSerialNoLicensed();
    void createModmanWithStorage();
    void startModman(QString sessionFileName);
    QEvent *generateEvent(int entityId, QString componentName, QVariant oldValue, QVariant newValue);

    QString readEntitiesAndCompoFromJsonFile(QString filePath);
    QJsonObject getStoredValueWithoutTimeStamp(int storageNum);
    QHash<QString, QVariant> getComponentsStoredOfEntity(int entityId, QJsonObject storedValueWithoutTimeStamp);
    QString getValuesStoredOfComponent(QHash<QString, QVariant> componentHash, QString componentName);
    void startLogging(QString fileName, int storageNum);
    void stopLogging(int storageNum);
    void changeRMSValues(QVariant newValue1, QVariant newValue2);

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    std::unique_ptr<Vf_Storage> m_vfStorageEntity;
    VeinEvent::StorageSystem* m_storage;

    bool m_dataLoggerSystemInitialized = false;
};

#endif // TEST_VFSTORAGE_H
