#ifndef TEST_VF_STORAGE_GUI_H
#define TEST_VF_STORAGE_GUI_H

#include "vf_storage.h"
#include "ve_eventhandler.h"
#include "vs_storageeventsystem.h"
#include <memory>
#include <QObject>

class test_vf_storage_gui : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void componentsFound();
    void storeValuesBasedOnNoEntitiesInJson();
    void storeValuesBasedOnIncorrectEntitiesInJson();
private:
    void changeComponentValue(int entityId, QString componentName, QVariant oldValue, QVariant newValue);
    QEvent *generateEvent(int entityId, QString componentName, QVariant oldValue, QVariant newValue);

    QString readEntitiesAndCompoFromJsonFile(QString filePath);
    void startLoggingFromJson(QString fileName, int storageNum);

    std::unique_ptr<VeinEvent::EventHandler> m_eventHandler;
    std::unique_ptr<Vf_Storage> m_storageRecorder;
    std::shared_ptr<VeinStorage::StorageEventSystem> m_storageEventSystem;
};

#endif // TEST_VF_STORAGE_GUI_H
