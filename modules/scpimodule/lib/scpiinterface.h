#ifndef SCPIINTERFACE_H
#define SCPIINTERFACE_H

#include "scpi.h"
#include "scpidelegatetemplate.h"

namespace SCPIMODULE
{

class cSCPIClient;

class cSCPIInterface: public QObject
{
    Q_OBJECT

public:
    explicit cSCPIInterface(const QString &name);

    void exportSCPIModelXML(QString &xml, QMap<QString, QString> modelListBaseEntry);
    void addSCPICommand(ScpiBaseDelegatePtr delegate);
    bool executeCmd(cSCPIClient* client, const QString &cmd, const ScpiTransactionId &scpiTransactionId);
    bool hasAmbiguousShortNames();

private:
    static ScpiAmbiguityMap ignoreAmbiguous(ScpiAmbiguityMap inMap);

    QString m_sName;
    cSCPI m_scpiCmdInterface;
};
}


#endif // SCPIINTERFACE_H
