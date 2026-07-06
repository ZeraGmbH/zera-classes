#ifndef SCPIMODULEINTERFACE_H
#define SCPIMODULEINTERFACE_H

#include "scpigroupbase.h"
#include "scpiinterface.h"
#include "scpibasedelegate.h"
#include "scpimeasuredelegate.h"
#include "scpicmdinfo.h"
#include <QHash>
#include <QJsonObject>
#include <QJsonArray>

namespace SCPIMODULE
{

class cSCPIModule;
class cSCPICatalogCmdDelegate;

// the class for our module interface connections
class ScpiGroupMeasureAndFriends : public ScpiGroupBase
{
    Q_OBJECT
public:
    ScpiGroupMeasureAndFriends(cSCPIModule* module, cSCPIInterface* iface);
    ~ScpiGroupMeasureAndFriends() override;

    bool setupScpi();
    virtual void actualizeInterface(QVariant modInterface);

    QHash<QString, cSCPIMeasureDelegatePtr>* getSCPIMeasDelegateHash();

private:
    cSCPIModule* m_pModule = nullptr;
    QHash<QString, cSCPIMeasureDelegatePtr> m_scpiMeasureDelegateHash; // a hash for measure cmd's ... needed for clean up and search for existing cmd
    QHash<QString, cSCPICatalogCmdDelegate*> m_scpiPropertyDelegateHash; // a hash with property delegates taht might need actualization when something changes

    void addSCPICommand(const cSCPICmdInfoPtr &scpiCmdInfo);
    void addRPCCommand(const cSCPICmdInfoPtr &scpiCmdInfo);
    void addSCPIMeasureCommand(const QString &cmdparent,
                               const QString &cmd,
                               quint8 scpiCmdQueryFlags,
                               ScpiModelTypes modelType,
                               cSCPIMeasure* measureObject,
                               QJsonObject veinComponentInfo = QJsonObject());
    void setXmlComponentInfo(ScpiBaseDelegatePtr delegate, const QJsonObject &componentInfo);
    void setXmlComponentValidatorInfo(ScpiBaseDelegatePtr delegate, const QJsonObject &componentInfo);
    QJsonArray getValidatorEntries(QJsonObject validator);

    QList<cSCPIMeasure*> m_measureObjectsToDelete;
};

}

#endif // SCPIMODULEINTERFACE_H
