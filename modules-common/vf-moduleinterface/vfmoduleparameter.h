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
                      const QString &componentname,
                      const QString &description,
                      const QVariant &initval,
                      bool deferredNotification = false);
    virtual ~VfModuleParameter();

    bool isValidParameter(QVariant& value);

    virtual void exportMetaData(QJsonObject &jsObj) override;
    void setValidator(ValidatorInterface* validator);
    virtual void veinTransaction(const QUuid &clientId,
                                 const QVariant &newValue,
                                 const QVariant &oldValue,
                                 VeinComponent::ComponentData::Command vccmd);

protected:
    void handleNoFetchVeinTransaction(QVariant newValue);

private:
    const bool m_bDeferredNotification;
    ValidatorInterface* m_pValidator;
};

#endif // VEINMODULEPARAMETER_H
