#ifndef SCPIPROPERTYDELEGATE
#define SCPIPROPERTYDELEGATE

#include <QString>

#include "scpidelegate.h"

namespace SCPIMODULE
{

class cSCPIModule;
class cSCPICmdInfo;

class cSCPIPropertyDelegate: public cSCPIDelegate
{
    Q_OBJECT

public:
    cSCPIPropertyDelegate(QString cmdParent, QString cmd, quint8 type, cSCPIModule* scpimodule, cSCPICmdInfo* scpicmdinfo);
    virtual ~cSCPIPropertyDelegate();

    virtual bool executeSCPI(cSCPIClient *client, QString& sInput);
    virtual void setOutput(cSCPICmdInfo* scpicmdinfo);
    virtual void setOutput(QVariant modInterface);

private:
    cSCPIModule* m_pModule;
    cSCPICmdInfo* m_pSCPICmdInfo;

    QString m_sAnswer;
};

}

#endif // SCPIPROPERTYDELEGATE

