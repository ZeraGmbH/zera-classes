#ifndef STATUSINTERFACE_H
#define STATUSINTERFACE_H

#include <QString>
#include <QList>

#include "baseinterface.h"

namespace SCPIMODULE
{


class cSCPIStatusDelegate;
class cSCPIClient;

// the class for our interface interface connections
// means special functions belonging to the interface itself

class cStatusInterface: public cBaseInterface
{
    Q_OBJECT

public:
    cStatusInterface(cSCPIModule* module, cSCPIInterface* iface);
    virtual ~cStatusInterface();

    virtual bool setupInterface();

private:
    QList<cSCPIStatusDelegate*> m_scpiStatusDelegateList; // our delegatelist for parameter cmd's ... needed to clean up

private slots:
    void executeCmd(cSCPIClient *client, int cmdCode, int statIndex, const QString &sInput);

};

}

#endif // STATUSINTERFACE_H
