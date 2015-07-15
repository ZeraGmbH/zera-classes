#include "veinmoduleparameter.h"
#include "paramvalidator.h"


cVeinModuleParameter::cVeinModuleParameter(int entityId, VeinEvent::EventSystem *eventsystem, QString name, QString description, QVariant initval, bool deferredNotification)
    :cVeinModuleComponent(entityId, eventsystem, name, description, initval), m_bDeferredNotification(deferredNotification)
{
}


void cVeinModuleParameter::exportMetaData(QJsonArray &jsArr)
{
    QJsonObject jsonObj;

    exportComponentMetaData(jsonObj);
    m_pValidator->exportMetaData(jsonObj);

    jsArr.append(jsonObj);
}


void cVeinModuleParameter::setValidator(cParamValidator *validator)
{
    m_pValidator = validator;
}


bool cVeinModuleParameter::transaction(QVariant newValue, QVariant oldValue)
{

}
