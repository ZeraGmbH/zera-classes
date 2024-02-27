#ifndef BASESAMPLECHANNEL_H
#define BASESAMPLECHANNEL_H

#include "moduleactivist.h"
#include <pcbinterface.h>
#include <rminterface.h>

class cBaseSampleChannel: public cModuleActivist
{
    Q_OBJECT

public:
    cBaseSampleChannel(QString name, quint8 chnnr);
    virtual ~cBaseSampleChannel(){}
    QString getName();
    QString getAlias();
protected:
    QString m_sName; // the channel's system name
    quint8 m_nChannelNr; // the number of our channel for naming purpose

    Zera::cRMInterface m_rmInterface;
    Zera::PcbInterfacePtr m_pPCBInterface;

    // the things we have to query from our pcb server
    QString m_sAlias; // the channel' alias
    QString m_sUnit; // we don't need but for api satisfaction ...
    QString m_sDescription; // we take description from resource manager
    quint16 m_nPort; // and also the port adress of pcb server
    QHash<quint32, int> m_MsgNrCmdList;

protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer) = 0;
};


#endif // BASESAMPLECHANNEL_H
