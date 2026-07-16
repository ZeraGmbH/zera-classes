#ifndef SCPICMDINFO_H
#define SCPICMDINFO_H

#include <QStringList>
#include <QJsonObject>
#include <memory>

namespace SCPIMODULE
{

class cSCPICmdInfo
{
public:
    QString scpiFullPath() const { return QString("%1:%2:%3").arg(scpiModel, scpiModuleName, scpiCommand); }
    QStringList scpiFullPathList() const { return scpiFullPath().split(":"); }
    QString scpiModuleName;
    QString scpiModel;
    QString scpiCommand;

    quint8 scpiQueryCmdFlags;

    int entityId;
    QString componentOrRpcName;

    QJsonObject veinComponentInfo;
};

typedef std::shared_ptr<cSCPICmdInfo> cSCPICmdInfoPtr;

}

#endif // SCPICMDINFO_H
