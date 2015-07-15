#ifndef VEINMODULEPARAMETER_H
#define VEINMODULEPARAMETER_H


#include <QString>
#include <QVariant>
#include <QJsonArray>

#include "veinmodulecomponent.h"

namespace VeinEvent
{
    class EventSystem;
}

class cParamValidator;

class cVeinModuleParameter: public cVeinModuleComponent
{
public:
    cVeinModuleParameter(int entityId, VeinEvent::EventSystem* eventsystem, QString name, QString description, QVariant initval, bool deferredNotification = false);

    virtual void exportMetaData(QJsonArray &jsArr);
    void setValidator(cParamValidator* validator);

    bool transaction(QVariant newValue, QVariant oldValue); // we return true if transaction is valid

signals:
    void sigValueChanged(QVariant); // we connect here if we want to do something on changed values

private:
    bool m_bDeferredNotification; //
    cParamValidator* m_pValidator;
};



#endif // VEINMODULEPARAMETER_H
