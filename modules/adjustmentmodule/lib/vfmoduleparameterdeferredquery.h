#ifndef VFMODULEPARAMETERDEFERREDQUERY_H
#define VFMODULEPARAMETERDEFERREDQUERY_H

#include <vfmoduleparameter.h>

class VfModuleParameterDeferredQuery : public VfModuleParameter
{
    Q_OBJECT
public:
    VfModuleParameterDeferredQuery(int entityId,
                                   VeinEvent::EventSystem *eventsystem,
                                   QString name,
                                   QString description,
                                   QVariant initval,
                                   bool deferredNotification = false);
    virtual void veinTransaction(QUuid clientId,
                                 QVariant newValue,
                                 QVariant oldValue,
                                 VeinComponent::ComponentData::Command vccmd) override;
signals:
    void sigValueQuery(QVariant newValue); // we connect here if we want to read a value before returning data from storage ...perhaps with parameter
};

#endif // VFMODULEPARAMETERDEFERREDQUERY_H
