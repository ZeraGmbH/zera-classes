#ifndef SCPIMODULEINTERFACE_H
#define SCPIMODULEINTERFACE_H

#include "baseinterface.h"
#include "scpibasedelegate.h"
#include "scpimeasuredelegate.h"
#include "scpicmdinfo.h"
#include <QHash>
#include <QJsonObject>
#include <QJsonArray>

namespace SCPIMODULE
{

namespace SCPIModelType {
    enum scpiModelType {normal, measure, configure, read, init, fetch };
}

class cSCPIModule;
class cSCPIInterface;
class cSCPIParameterDelegate;
class cSCPICatalogCmdDelegate;
class cSCPIMeasure;

// the class for our module interface connections

class cModuleInterface: public cBaseInterface
{
    Q_OBJECT
public:
    cModuleInterface(cSCPIModule* module, cSCPIInterface* iface);
    virtual ~cModuleInterface();

    virtual bool setupInterface() override;
    virtual void actualizeInterface(QVariant modInterface);

    QHash<QString, cSCPIMeasureDelegatePtr>* getSCPIMeasDelegateHash();

private:
    QHash<QString, cSCPIMeasureDelegatePtr> m_scpiMeasureDelegateHash; // a hash for measure cmd's ... needed for clean up and search for existing cmd
    QHash<QString, cSCPICatalogCmdDelegate*> m_scpiPropertyDelegateHash; // a hash with property delegates taht might need actualization when something changes

    void addSCPICommand(cSCPICmdInfoPtr scpiCmdInfo);
    void addSCPIMeasureCommand(QString cmdparent,
                               QString cmd,
                               quint8 cmdType,
                               quint8 measCode,
                               cSCPIMeasure* measureObject,
                               QJsonObject veinComponentInfo = QJsonObject());
    void setXmlComponentInfo(ScpiBaseDelegatePtr delegate, const QJsonObject &componentInfo);
    void setXmlComponentValidatorInfo(ScpiBaseDelegatePtr delegate, const QJsonObject &componentInfo);
    QJsonArray getValidatorEntries(QJsonObject validator);

    QList<cSCPIMeasure*> m_measureObjectsToDelete;
};

}

#endif // SCPIMODULEINTERFACE_H
