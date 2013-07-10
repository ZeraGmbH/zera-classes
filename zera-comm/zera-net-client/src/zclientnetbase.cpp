#include "zclientnetbase.h"

#include "zclientnetbase_private.h"
namespace Zera
{
  namespace NetClient
  {
    ZeraClientNetBase::ZeraClientNetBase(QObject *parent) :
      QObject(parent), d_ptr(new _ClientNetBasePrivate(this))
    {
      connect(d_ptr, SIGNAL(connectionLost()), this, SIGNAL(connectionLost()));
      connect(d_ptr, SIGNAL(messageAvailable(QByteArray)), this, SIGNAL(messageAvailable(QByteArray)));

    }

    bool ZeraClientNetBase::readMessage(google::protobuf::Message *message, const QByteArray &array)
    {
      return _ClientNetBasePrivate::readMessage(message,array);
    }

    void ZeraClientNetBase::sendMessage(google::protobuf::Message *message)
    {
      d_ptr->sendMessage(message);
    }

    void ZeraClientNetBase::startNetwork(QString ipAddress, quint16 port)
    {
      d_ptr->startNetwork(ipAddress,port);
    }
  }
}
