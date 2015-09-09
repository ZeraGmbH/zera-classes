#ifndef SCPIPARAMETERDELEGATE_H
#define SCPIPARAMETERDELEGATE_H

#include <QString>

#include "scpidelegate.h"

namespace SCPIMODULE
{

class cSCPIModule;
class cSCPICmdInfo;

class cSCPIParameterDelegate: public cSCPIDelegate
{
    Q_OBJECT

public:
    cSCPIParameterDelegate(QString cmdParent, QString cmd, quint8 type, cSCPIModule* scpimodule, cSCPICmdInfo* scpicmdinfo);
    virtual ~cSCPIParameterDelegate();

    virtual bool executeSCPI(cSCPIClient *client, QString& sInput);

private:
    cSCPIModule* m_pModule;
    cSCPICmdInfo* m_pSCPICmdInfo;
};

}
#endif // SCPIPARAMETERDELEGATE_H
