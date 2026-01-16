#ifndef RANGEMODULEVALUEOBSERVER_H
#define RANGEMODULEVALUEOBSERVER_H

#include <QObject>
#include <vs_abstracteventsystem.h>

static constexpr int RangeModuleEntityId = 1020;

class RangeModuleValueObserver : public QObject
{
    Q_OBJECT
public:
    RangeModuleValueObserver(VeinStorage::AbstractEventSystem* veinStorage);
    float getRelativeRangeValue(const QString &channelMName) const;
signals:
    void sigNewValues();

private slots:
    void onRangeSigMeasChange(QVariant newValue);
private:
    void createModule(int entityId, QMap<QString, QVariant> components);
    static bool isRejValueForClampMissingOnClampOnlyChannel(float rejValue);

    VeinStorage::AbstractEventSystem *m_veinStorage;
    VeinStorage::StorageComponentPtr m_rangeMeasSignalComponent;
};

#endif // RANGEMODULEVALUEOBSERVER_H
