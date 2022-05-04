#ifndef VEINMODULEPARAMETER_H
#define VEINMODULEPARAMETER_H

#include "veinmodulecomponent.h"
#include "scpiinfo.h"
#include "paramvalidator.h"
#include <QUuid>

class cVeinModuleParameter: public cVeinModuleComponent
{
    Q_OBJECT
public:
    cVeinModuleParameter(int entityId,
                         VeinEvent::EventSystem *eventsystem,
                         QString name,
                         QString description,
                         QVariant initval,
                         bool deferredNotification = false,
                         bool deferredQueryNotification = false);
    virtual ~cVeinModuleParameter();

    bool hasDeferredNotification();
    bool hasDeferredQueryNotification();
    bool isValidParameter(QVariant& value);

    virtual void exportMetaData(QJsonObject &jsObj);
    virtual void exportSCPIInfo(QJsonArray &jsArr);
    void setSCPIInfo(cSCPIInfo* scpiinfo);
    void setValidator(cParamValidator* validator);
    void transaction(QUuid clientId, QVariant newValue, QVariant oldValue, VeinComponent::ComponentData::Command vccmd);

private:
    bool m_bDeferredNotification; //
    bool m_bDeferredQueryNotification;
    cParamValidator* m_pValidator;
    cSCPIInfo* m_pscpiInfo;
};

#endif // VEINMODULEPARAMETER_H
