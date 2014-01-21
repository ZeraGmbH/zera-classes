#ifndef INTERFACE_H
#define INTERFACE_H

#include <QObject>
#include <QVariant>

#include "pcbproxy_global.h"


namespace Zera
{
namespace PCBProxy
{

enum replies
{
    ack,
    nack,
    error,
    debug,
    ident
};


class ZERAPCBPROXYSHARED_EXPORT cInterface: public QObject
{
    Q_OBJECT

public:
    virtual void getDSPChannel(QString chnName) = 0; // int
    virtual void getStatus(QString chnName) = 0; // int
    virtual void getAlias(QString chnName) = 0; // qstring
    virtual void getType(QString chnName) = 0; // qstring
    virtual void getUnit(QString chnName) = 0; // qstring
    virtual void getRange(QString chnName) = 0; // qstring
    virtual void getRangeList(QString chnName) = 0; // qstringlist
    virtual void getAlias(QString chnName, QString rngName) = 0; // qstring
    virtual void getType(QString chnName, QString rngName) = 0; // int
    virtual void getUrvalue(QString chnName, QString rngName) = 0; // double
    virtual void getRejection(QString chnName, QString rngName) = 0; // double
    virtual void getOVRejection(QString chnName, QString rngName) = 0; // double
    virtual void isAvail(QString chnName, QString rngName) = 0; // bool

    virtual void setRange(QString chnName, QString rngName) = 0; // reply (ack, nak..)

signals:
    void pcbReply(replies reply);
    void pcbAnswer(QVariant answer);
};

}
}

#endif // INTERFACE_H
