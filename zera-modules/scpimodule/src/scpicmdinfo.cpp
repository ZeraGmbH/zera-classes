#include "scpicmdinfo.h"


namespace SCPIMODULE
{


cSCPICmdInfo::cSCPICmdInfo(const SCPIMODULE::cSCPICmdInfo &obj)
{
    scpiModuleName = obj.scpiModuleName;
    scpiModel = obj.scpiModel;
    scpiCommand = obj.scpiCommand;
    scpiCommandType = obj.scpiCommandType;
    unit = obj.unit;
    entityId = obj.entityId;
    componentName = obj.componentName;
    refType = obj.refType;
}

}
