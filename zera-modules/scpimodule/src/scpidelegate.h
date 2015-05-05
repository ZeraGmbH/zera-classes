#ifndef SCPIDELEGATE_H
#define SCPIDELEGATE_H

#include <QObject>
#include <QString>

#include "scpiobject.h"

class cSCPI;

namespace SCPIMODULE
{

class cSCPIClient;

class cSCPIDelegate: public QObject, public cSCPIObject
{
   Q_OBJECT

public:
    cSCPIDelegate(QString cmdParent, QString cmd, quint8 type, cSCPI *scpiInterface);
    virtual bool executeSCPI(const QString&, QString&);
    virtual bool executeSCPI(cSCPIClient *client, const QString& sInput) = 0;

};

}
#endif // SCPIDELEGATE_H
