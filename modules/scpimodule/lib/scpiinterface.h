#ifndef SCPIINTERFACE_H
#define SCPIINTERFACE_H

#include "scpi.h"
#include "scpibasedelegate.h"
#include <QObject>
#include <QString>
#include <QList>
#include <QQueue>
#include <timerfactoryqt.h>

class cSCPI;

namespace SCPIMODULE
{

class cSCPIClient;

struct cmdInfos {
    QString cmd;
    cSCPIClient* client;
};

class cSCPIInterface: public QObject
{
    Q_OBJECT

public:
    cSCPIInterface(){}
    cSCPIInterface(QString name);
    virtual ~cSCPIInterface();

    void exportSCPIModelXML(QString &xml, QMap<QString, QString> modelListBaseEntry);
    void addSCPICommand(ScpiBaseDelegatePtr delegate);
    bool executeCmd(cSCPIClient* client, QString cmd);
    void checkAmbiguousShortNames();
    void setEnableQueue(bool enable);

private:
    static ScpiAmbiguityMap ignoreAmbiguous(ScpiAmbiguityMap inMap);
    void removeCommand(cSCPIClient *client);
    void checkAllCmds();
    void waitForBlockingCmd(cSCPIClient *client);

    QString m_sName;
    cSCPI* m_pSCPICmdInterface;
    QQueue<cmdInfos> m_scpiCmdInExec;
    TimerTemplateQtPtr m_expCmd;
    bool m_enableScpiQueue;
};
}


#endif // SCPIINTERFACE_H
