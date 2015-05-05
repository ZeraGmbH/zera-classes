#ifndef SCPIINTERFACE_H
#define SCPIINTERFACE_H

#include <QObject>
#include <QList>
#include <QVector>
#include <QHash>

class cSCPI;

namespace SCPIMODULE
{

enum scpiinterfacecommands
{
    deviceinterfacecmd
};


namespace SCPIModelType {
    enum scpiModelType {normal, measure, configure, read, init, fetch };
}

class cSCPIInterfaceDelegate;
class cSCPIParameterDelegate;
class cSCPIMeasureDelegate;
class cSCPIMeasure;
class cSCPICmdInfo;
class cSCPIClient;

class cSCPIInterface: public QObject
{
    Q_OBJECT

public:
    cSCPIInterface(){}
    cSCPIInterface(QString name);
    virtual ~cSCPIInterface();

    void addSCPICommand(cSCPICmdInfo& scpiCmdInfo);
    bool executeCmd(cSCPIClient* client, QString cmd);


private:
    QString m_sName;
    cSCPI* m_pSCPICmdInterface;

    QList<cSCPIInterfaceDelegate*> m_scpiInterfaceDelegateList; // our delegatelist for interface cmd's ... needed to clean up
    QList<cSCPIParameterDelegate*> m_scpiParameterDelegateList; // our delegatelist for parameter cmd's ... needed to clean up
    QHash<QString, cSCPIMeasureDelegate*> m_scpiMeasureDelegateHash; // a hash for measure cmd's ... needed for clean up and search for existing cmd

    void addSCPIMeasureCommand(QString cmdparent, QString cmd,  quint8 cmdType, quint8 measCode, cSCPIMeasure* measureObject);

private slots:
    void executeCmd(cSCPIClient* client, int cmdCode, QString& sInput);
};

}


#endif // SCPIINTERFACE_H
