#include "zeraclientprivate.h"


#include <QState>
#include <QFinalState>
#include <QDebug>

namespace Zera
{
  namespace Net
  {
    cClientPrivate::cClientPrivate(quint32 socketDescriptor) :
      sockDescriptor(socketDescriptor)
    {
    }
  }
}
