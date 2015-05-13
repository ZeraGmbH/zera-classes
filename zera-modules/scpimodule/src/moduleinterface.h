#ifndef MODULEINTERFACE_H
#define MODULEINTERFACE_H

#include <QObject>
#include <QHash>

#include "baseinterface.h"

class VeinPeer;

namespace SCPIMODULE
{

namespace SCPIModelType {
    enum scpiModelType {normal, measure, configure, read, init, fetch };
}

class cSCPICmdInfo;
class cSCPIInterface;
class cSCPIParameterDelegate;
class cSCPIMeasure;
class cSCPIMeasureDelegate;

// the class for our module interface connections

class cModuleInterface: public cBaseInterface
{
    Q_OBJECT

public:
    cModuleInterface(VeinPeer* peer, cSCPIInterface* iface);
    virtual ~cModuleInterface();

    virtual bool setupInterface();

private:
    QList<cSCPIParameterDelegate*> m_scpiParameterDelegateList; // our delegatelist for parameter cmd's ... needed to clean up
    QHash<QString, cSCPIMeasureDelegate*> m_scpiMeasureDelegateHash; // a hash for measure cmd's ... needed for clean up and search for existing cmd

    void addSCPICommand(cSCPICmdInfo &scpiCmdInfo);
    void addSCPIMeasureCommand(QString cmdparent, QString cmd,  quint8 cmdType, quint8 measCode, cSCPIMeasure* measureObject);


};

}

#endif // MODULEINTERFACE_H
