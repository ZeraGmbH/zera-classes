#ifndef MODULEINTERFACE_H
#define MODULEINTERFACE_H

#include <QObject>
#include <QHash>

#include "baseinterface.h"

namespace SCPIMODULE
{

namespace SCPIModelType {
    enum scpiModelType {normal, measure, configure, read, init, fetch };
}

class cSCPIModule;
class cSCPICmdInfo;
class cSCPIInterface;
class cSCPIDelegate;
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
    QList<cSCPIDelegate*> m_scpiDelegateList; // our delegatelist for parameter cmd's ... needed to clean up
    QHash<QString, cSCPIMeasureDelegate*> m_scpiMeasureDelegateHash; // a hash for measure cmd's ... needed for clean up and search for existing cmd
    QHash<QString, cSCPIPropertyDelegate*> m_scpiPropertyDelegateHash; // a hash with property delegates taht might need actualization when something changes

    void addSCPICommand(cSCPICmdInfo *scpiCmdInfo);
    void addSCPIMeasureCommand(QString cmdparent, QString cmd,  quint8 cmdType, quint8 measCode, cSCPIMeasure* measureObject);


};

}

#endif // MODULEINTERFACE_H
