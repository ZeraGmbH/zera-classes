#include "zeraserverprivate.h"
#include "zeraclient.h"

namespace Zera
{
  namespace Net
  {
    cServer* cServerPrivate::singletonInstance=0;

    cServerPrivate::cServerPrivate(cServer *parent) :
      q_ptr(parent)
    {
    }

    cServerPrivate::~cServerPrivate()
    {
      foreach(Zera::Net::cClient* c, clients)
      {
        clients.removeAll(c);
        c->deleteLater();
      }
    }
  }
}
