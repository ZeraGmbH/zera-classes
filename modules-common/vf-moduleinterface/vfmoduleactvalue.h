#ifndef VEINMODULEACTVALUE_H
#define VEINMODULEACTVALUE_H

#include "vfmodulecomponent.h"
#include <scpiveincomponentinfo.h>

class VfModuleActvalue: public VfModuleComponent
{
    Q_OBJECT
public:
    VfModuleActvalue(int entityId, VeinEvent::EventSystem *eventsystem, QString name, QString description, QVariant initval = QVariant());
    virtual void exportSCPIInfo(QJsonArray &jsArr);
    void setScpiInfo(const QString &model,
                     const QString &cmd,
                     int cmdTypeMask, // e.g SCPI::isQuery|SCPI::isCmdwP
                     const QString &veinComponentName,
                     SCPI::eSCPIEntryType entryType);
private:
    std::unique_ptr<ScpiVeinComponentInfo> m_scpiInfo;
};


#endif // VEINMODULEACTVALUE_H
