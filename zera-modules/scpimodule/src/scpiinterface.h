#ifndef SCPIINTERFACE_H
#define SCPIINTERFACE_H

#include <QObject>
#include <QList>
#include <QVector>
#include <QHash>

class cSCPI;

namespace SCPIMODULE
{

namespace SCPIModelType {
    enum scpiModelType {normal, measure, configure, read, init, fetch };
}


class cSCPInonMeasureDelegate;
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

    QList<cSCPInonMeasureDelegate*> m_scpinonMeasureDelegateList; // our delegatelist for non measure cmd's ... needed to clean up
    QHash<QString, cSCPIMeasureDelegate*> m_scpiMeasureDelegateHash; // a hash for measure cmd's ... needed for clean up and search for existing cmd

    quint32 m_nCmdCode;

    void addSCPIMeasureCommand(QString cmdparent, QString cmd,  quint8 cmdType, quint16 cmdCode, cSCPIMeasure* measureObject);

private slots:
    void executeCmd(cSCPIClient* client, int cmdCode, QString& sInput);
};

}


#endif // SCPIINTERFACE_H
