#ifndef SCPIINTERFACE_H
#define SCPIINTERFACE_H

#include "scpi.h"
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
    void checkAmbiguousShortNames();

private:
    static ScpiAmbiguityMap ignoreAmbiguous(ScpiAmbiguityMap inMap);
    QString m_sName;
    cSCPI* m_pSCPICmdInterface;
};

}


#endif // SCPIINTERFACE_H
