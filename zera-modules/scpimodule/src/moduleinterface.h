#ifndef SCPIMODULEINTERFACE_H
#define SCPIMODULEINTERFACE_H

#include "baseinterface.h"
#include <QHash>
#include <QJsonObject>
#include <QJsonArray>

namespace SCPIMODULE
{

namespace SCPIModelType {
    enum scpiModelType {normal, measure, configure, read, init, fetch };
}

class cSCPIModule;
class cSCPICmdInfo;
class cSCPIInterface;
class ScpiBaseDelegate;
class cSCPIParameterDelegate;
class cSCPIPropertyDelegate;
class cSCPIMeasure;
class cSCPIMeasureDelegate;

// the class for our module interface connections

class cModuleInterface: public cBaseInterface
{
    Q_OBJECT

public:
    cModuleInterface(cSCPIModule* module, cSCPIInterface* iface);
    virtual ~cModuleInterface();

    virtual bool setupInterface();
    virtual void actualizeInterface(QVariant modInterface);

    QHash<QString, cSCPIMeasureDelegate*>* getSCPIMeasDelegateHash();

private:
    QList<ScpiBaseDelegate*> m_scpiDelegateList; // our delegatelist for parameter cmd's ... needed to clean up
    QHash<QString, cSCPIMeasureDelegate*> m_scpiMeasureDelegateHash; // a hash for measure cmd's ... needed for clean up and search for existing cmd
    QHash<QString, cSCPIPropertyDelegate*> m_scpiPropertyDelegateHash; // a hash with property delegates taht might need actualization when something changes

    void addSCPICommand(cSCPICmdInfo *scpiCmdInfo);
    void addSCPIMeasureCommand(QString cmdparent, QString cmd,  quint8 cmdType, quint8 measCode, cSCPIMeasure* measureObject, QJsonObject veinComponentInfo = QJsonObject());
    void setXmlComponentInfo(ScpiBaseDelegate* delegate, const QJsonObject &componentInfo);
    void setXmlComponentValidatorInfo(ScpiBaseDelegate* delegate, const QJsonObject &componentInfo);
    QJsonArray getValidatorEntries(QJsonObject validator);
};

}

#endif // SCPIMODULEINTERFACE_H
