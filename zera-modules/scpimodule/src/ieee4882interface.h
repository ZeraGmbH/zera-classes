#ifndef IEEE4882INTERFACE_H
#define IEEE4882INTERFACE_H

#include <QObject>
#include <QString>

#include "baseinterface.h"


namespace SCPIMODULE
{

class cSCPIInterface;
class cSCPIInterfaceDelegate;
class cSCPIClient;

// the class for our module interface connections

class cIEEE4882Interface: public cBaseInterface
{
    Q_OBJECT

public:
    cIEEE4882Interface(cSCPIModule* module, cSCPIInterface* iface);
    virtual ~cIEEE4882Interface();

    virtual bool setupInterface();

private:
    QList<cSCPIInterfaceDelegate*> m_IEEE4882DelegateList; // our delegatelist for parameter cmd's ... needed to clean up

private slots:
    void executeCmd(cSCPIClient* client, int cmdCode, const QString& sInput);
};

}

#endif // IEEE4882INTERFACE_H
