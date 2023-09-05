#ifndef SCPIINTERFACE_H
#define SCPIINTERFACE_H

#include <QObject>
#include <QString>
#include <QList>
#include <QQueue>
#include <timerfactoryqt.h>

class cSCPI;

namespace SCPIMODULE
{

class ScpiBaseDelegate;
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
    void addSCPICommand(ScpiBaseDelegate* delegate);
    bool executeCmd(cSCPIClient* client, QString cmd);
    void setEnableQueue(bool enable);

private:
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
