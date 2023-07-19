#ifndef SCPIINTERFACE_H
#define SCPIINTERFACE_H

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>

class cSCPI;
namespace SCPIMODULE
{
class cSCPIClient;
class ScpiBaseDelegate;

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
    void removeCommand(cSCPIClient* client);
    void waiForAnswer();
    bool checkAllCmds();
    QString m_sName;
    cSCPI* m_pSCPICmdInterface;
    QList<cmdInfos> m_scpiCmdInExec;
    QList<cmdInfos> m_scpiCmdsWaiting;
};

}


#endif // SCPIINTERFACE_H
