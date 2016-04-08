#ifndef SCPICMDINFO_H
#define SCPICMDINFO_H

#include <QString>

namespace SCPIMODULE
{

class cSCPICmdInfo
{
public:
    cSCPICmdInfo(){}
    QString scpiModuleName;
    QString scpiModel;
    QString scpiCommand;
    QString scpiCommandType;
    QString unit;
    int entityId;
    QString componentName;
    QString refType; // 0 means component itself otherwise the component's validation data
};

}

#endif // SCPICMDINFO_H
