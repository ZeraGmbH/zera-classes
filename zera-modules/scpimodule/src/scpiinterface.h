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


class cSCPIInterface: public QObject
{
    Q_OBJECT

public:
    cSCPIInterface(QString name);
    cSCPIInterface(const cSCPIInterface& other);
    virtual ~cSCPIInterface();

    void setAuthorisation(bool auth);
    void addSCPICommand(cSCPICmdInfo& scpiCmdInfo);
    bool executeCmd(QString cmd);

signals:
    void cmdStatus(quint8);
    void cmdAnswer(QString);

private:
    QString m_sName;
    bool m_bAuthorized; // default false
    cSCPI* m_pSCPICmdInterface;

    QList<cSCPInonMeasureDelegate*> m_scpinonMeasureDelegateList; // our delegatelist for non measure cmd's ... needed to clean up
    QHash<QString, cSCPIMeasureDelegate*> m_scpiMeasureDelegateHash; // a hash for measure cmd's ... needed for clean up and search for existing cmd

    quint32 m_nCmdCode;

    void addSCPIMeasureCommand(QString cmdparent, QString cmd,  quint8 cmdType, quint16 cmdCode, cSCPIMeasure* measureObject);

private slots:
    void executeCmd(int cmdCode, QString& sInput);
};

}


#endif // SCPIINTERFACE_H
