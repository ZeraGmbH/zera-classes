#include "zeraclientprivate.h"


#include "zeraclient.h"
#include <QState>
#include <QFinalState>
#include <QDebug>

namespace Zera
{
  namespace Net
  {
    cClientPrivate::cClientPrivate(quint32 socketDescriptor, cClient *parent) :
      sockDescriptor(socketDescriptor),
      q_ptr(parent)
    {
      stContainer = new QState(parent);
      stInit = new QState(stContainer);
      stConnected = new QState(stContainer);
      stAboutToDisconnect = new QState(stContainer);
      fstDisconnected = new QFinalState(parent);
    }
  }
}
