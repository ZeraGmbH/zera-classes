#ifndef VEINMODULEPARAMETER_H
#define VEINMODULEPARAMETER_H

#include "vfmodulecomponent.h"
#include "validatorinterface.h"
#include <QUuid>

class VfModuleParameter: public VfModuleComponent
{
    Q_OBJECT
public:
    VfModuleParameter(int entityId,
                      VeinEvent::EventSystem *eventsystem,
                      QString name,
                      QString description,
                      QVariant initval,
                      bool deferredNotification = false);
    virtual ~VfModuleParameter();

    bool isValidParameter(QVariant& value);

    virtual void exportMetaData(QJsonObject &jsObj);
    void setValidator(ValidatorInterface* validator);
    virtual void veinTransaction(QUuid clientId,
                                 QVariant newValue,
                                 QVariant oldValue,
                                 VeinComponent::ComponentData::Command vccmd);

protected:
    void handleNoFetchVeinTransaction(QVariant newValue);

private:
    const bool m_bDeferredNotification;
    ValidatorInterface* m_pValidator;
};

#endif // VEINMODULEPARAMETER_H
