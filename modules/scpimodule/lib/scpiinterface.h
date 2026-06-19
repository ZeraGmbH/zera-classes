#ifndef SCPIINTERFACE_H
#define SCPIINTERFACE_H

#include "scpi.h"
#include "scpibasedelegate.h"

namespace SCPIMODULE
{

class cSCPIClient;

class cSCPIInterface: public QObject
{
    Q_OBJECT

public:
    cSCPIInterface(){}
    cSCPIInterface(const QString &name);
    virtual ~cSCPIInterface();

    void exportSCPIModelXML(QString &xml, QMap<QString, QString> modelListBaseEntry);
    void addSCPICommand(ScpiBaseDelegatePtr delegate);
    bool executeCmd(cSCPIClient* client, const QString &cmd, const ScpiTransactionId &scpiTransactionId);
    void checkAmbiguousShortNames();

private:
    static ScpiAmbiguityMap ignoreAmbiguous(ScpiAmbiguityMap inMap);

    QString m_sName;
    cSCPI* m_pSCPICmdInterface;
};
}


#endif // SCPIINTERFACE_H
