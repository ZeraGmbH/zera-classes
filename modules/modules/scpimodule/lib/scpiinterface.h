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


private:
    void removeCommand(cSCPIClient *client);
    bool checkAllCmds();
    QString m_sName;
    cSCPI* m_pSCPICmdInterface;
    QQueue<cmdInfos> m_scpiCmdInExec;
    TimerTemplateQtPtr m_expCmd;
    void waitForBlockingCmd(cSCPIClient *client);
};
}


#endif // SCPIINTERFACE_H
