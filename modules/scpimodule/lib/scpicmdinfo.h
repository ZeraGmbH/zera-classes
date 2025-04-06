#ifndef SCPICMDINFO_H
#define SCPICMDINFO_H

#include <QString>
#include <QJsonObject>

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
    int entityId;
    QString componentName;
    QString refType; // 0 means component itself otherwise the component's validation data

    QJsonObject veinComponentInfo;
};

}

#endif // SCPICMDINFO_H
