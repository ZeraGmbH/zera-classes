#ifndef SCPIINTERFACE_H
#define SCPIINTERFACE_H

#include <QObject>
#include <QString>
#include <QList>

class cSCPI;

namespace SCPIMODULE
{

class cSCPIDelegate;
class cSCPIClient;


class cSCPIInterface: public QObject
{
    Q_OBJECT

public:
    cSCPIInterface(){}
    cSCPIInterface(QString name);
    virtual ~cSCPIInterface();

    cSCPI* getSCPICmdInterface();
    void addSCPICommand(cSCPIDelegate* delegate);
    bool executeCmd(cSCPIClient* client, QString cmd);


private:
    QString m_sName;
    cSCPI* m_pSCPICmdInterface;
};

}


#endif // SCPIINTERFACE_H
