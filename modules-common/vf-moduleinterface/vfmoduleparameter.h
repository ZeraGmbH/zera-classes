#ifndef VEINMODULEPARAMETER_H
#define VEINMODULEPARAMETER_H

#include "vfmodulecomponent.h"
#include "scpiinfo.h"
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
                         bool deferredNotification = false,
                         bool deferredQueryNotification = false);
    virtual ~VfModuleParameter();

    bool hasDeferredNotification();
    bool hasDeferredQueryNotification();
    bool isValidParameter(QVariant& value);

    virtual void exportMetaData(QJsonObject &jsObj);
    virtual void exportSCPIInfo(QJsonArray &jsArr);
    void setSCPIInfo(cSCPIInfo* scpiinfo);
    void setValidator(ValidatorInterface* validator);
    void veinTransaction(QUuid clientId, QVariant newValue, QVariant oldValue, VeinComponent::ComponentData::Command vccmd);

private:
    bool m_bDeferredNotification; //
    bool m_bDeferredQueryNotification;
    ValidatorInterface* m_pValidator;
    cSCPIInfo* m_pscpiInfo;
};

#endif // VEINMODULEPARAMETER_H
