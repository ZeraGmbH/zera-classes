#ifndef SCPIINTERFACE_H
#define SCPIINTERFACE_H

#include <QObject>
#include <QString>
#include <QList>

class cSCPI;

namespace SCPIMODULE
{

class ScpiBaseDelegate;
class cSCPIClient;


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
    QString m_sName;
    cSCPI* m_pSCPICmdInterface;
};

}


#endif // SCPIINTERFACE_H
