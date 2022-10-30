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
    cSCPIDelegate();
    cSCPIDelegate(QString cmdParent, QString cmd, quint8 type);
    virtual ~cSCPIDelegate(){}

    void setCommand(cSCPI *scpiCmdInterface);
    virtual bool executeSCPI(const QString&, QString&) { return true; }; // cSCPIObject requires an override
    virtual bool executeSCPI(cSCPIClient *client, QString& sInput) = 0;

signals:
    void signalStatus(quint8);
    void signalAnswer(QString);

protected:
    QString m_sCmdParent;
};

}
#endif // SCPIDELEGATE_H
