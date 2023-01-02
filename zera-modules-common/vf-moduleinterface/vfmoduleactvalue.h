#ifndef VEINMODULEACTVALUE_H
#define VEINMODULEACTVALUE_H

#include "vfmodulecomponent.h"
#include "scpiinfo.h"

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
