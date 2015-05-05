#ifndef SCPIPARAMETERDELEGATE_H
#define SCPIPARAMETERDELEGATE_H

#include <QString>

#include "scpidelegate.h"

class VeinPeer;
class VeinEntity;

namespace SCPIMODULE
{

class cSCPIParameterDelegate: public cSCPIDelegate
{
    Q_OBJECT

public:
    cSCPIParameterDelegate(QString cmdParent, QString cmd, quint8 type, cSCPI *scpiInterface, VeinPeer* peer, VeinEntity* entity);

    virtual bool executeSCPI(cSCPIClient *client, const QString& sInput);

private:
    VeinPeer* m_pPeer;
    VeinEntity* m_pEntity;
};

}
#endif // SCPIPARAMETERDELEGATE_H
