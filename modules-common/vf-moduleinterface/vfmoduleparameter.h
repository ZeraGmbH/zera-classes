#ifndef VEINMODULEPARAMETER_H
#define VEINMODULEPARAMETER_H

#include "vfmodulecomponent.h"
#include "validatorinterface.h"
#include <scpiveincomponentinfo.h>
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
                         bool deferredNotification = false,
                         bool deferredQueryNotification = false);
    virtual ~VfModuleParameter();

    bool hasDeferredNotification();
    bool hasDeferredQueryNotification();
    bool isValidParameter(QVariant& value);

    virtual void exportMetaData(QJsonObject &jsObj);
    void setValidator(ValidatorInterface* validator);
    void veinTransaction(QUuid clientId, QVariant newValue, QVariant oldValue, VeinComponent::ComponentData::Command vccmd);
signals:
    void sigValueQuery(QVariant); // we connect here if we want to read a value before returning data from storage ...perhaps with parameter

private:
    bool m_bDeferredNotification; //
    bool m_bDeferredQueryNotification;
    ValidatorInterface* m_pValidator;
};

#endif // VEINMODULEPARAMETER_H
