#ifndef SCPICMDINFO_H
#define SCPICMDINFO_H

#include <QString>

class VeinPeer;
class VeinEntity;

namespace SCPIMODULE
{

class cSCPICmdInfo
{
public:
    cSCPICmdInfo(){}
    QString scpiModuleName;
    VeinPeer* peer;
    VeinEntity* entity;
    QString scpiModel;
    QString cmdNode;
    QString unit;
    QString type;
};

}

#endif // SCPICMDINFO_H
