#ifndef VEINMODULEACTVALUE_H
#define VEINMODULEACTVALUE_H

#include "vfmodulecomponent.h"
#include "scpiinfo.h"

class VfModuleActvalue: public VfModuleComponent
{
    Q_OBJECT
public:
    VfModuleActvalue(int entityId, VeinEvent::EventSystem *eventsystem, QString name, QString description, QVariant initval = QVariant());
    virtual ~VfModuleActvalue();
    virtual void exportSCPIInfo(QJsonArray &jsArr);
    void setSCPIInfo(cSCPIInfo* scpiinfo);
private:
    cSCPIInfo *m_pscpiInfo;
};


#endif // VEINMODULEACTVALUE_H
