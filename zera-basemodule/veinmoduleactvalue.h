#ifndef VEINMODULEACTVALUE_H
#define VEINMODULEACTVALUE_H

#include <QString>
#include <QVariant>
#include <QJsonArray>

class cSCPIInfo;

#include "veinmodulecomponent.h"

namespace VeinEvent
{
    class EventSystem;
    class StorageSystem;
}


class cVeinModuleActvalue: public cVeinModuleComponent
{
    Q_OBJECT
public:
    cVeinModuleActvalue(int entityId, VeinEvent::EventSystem *eventsystem, QString name, QString description, QVariant initval);
    virtual ~cVeinModuleActvalue();

    virtual void exportSCPIInfo(QJsonArray &jsArr);
    void setSCPIInfo(cSCPIInfo* scpiinfo);

private:
    cSCPIInfo *m_pscpiInfo;

};


#endif // VEINMODULEACTVALUE_H
