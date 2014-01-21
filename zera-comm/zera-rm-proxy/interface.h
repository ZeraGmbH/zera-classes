#ifndef INTERFACE_H
#define INTERFACE_H

#include <QObject>

#include "rmproxy_global.h"


namespace Zera
{
namespace RMProxy
{

enum replies
{
    ack,
    nack,
    error,
    debug,
    ident
};


class RMPROXYSHARED_EXPORT cInterface: public QObject
{
    Q_OBJECT

public:
    virtual void addResource(QString type, QString name, int n, QString description, quint16 port) = 0;
    virtual void removeResource(QString type, QString name) = 0;
    virtual void getResourceTypes() = 0;
    virtual void getResources(QString type) = 0;
    virtual void getResourceInfo(QString type, QString name) = 0;
    virtual void setResource(QString type, QString name, int n) = 0;
    virtual void freeResource(QString type, QString name) = 0;

signals:
    void rmReply(replies reply);
    void rmAnswer(QString message);
};

}
}


#endif // INTERFACE_H
