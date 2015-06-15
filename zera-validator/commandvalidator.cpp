#include "commandvalidator.h"

#include <QEvent>
#include <ve_commandevent.h>

namespace Zera
{

  CommandValidator::CommandValidator(QObject *t_parent) : VeinEvent::EventSystem(t_parent)
  {

  }

  bool CommandValidator::processEvent(QEvent *t_event)
  {
    bool retVal = false;


    if(t_event->type() == VeinEvent::CommandEvent::eventType())
    {
      VeinEvent::CommandEvent *cEvent = 0;
      cEvent = static_cast<VeinEvent::CommandEvent *>(t_event);
      if(cEvent != 0)
      {
        switch(cEvent->eventSubtype())
        {
          case VeinEvent::CommandEvent::EventSubtype::TRANSACTION:
          {
            retVal = true;
            processCommandEvent(cEvent);
            break;
          }
          default:
            break;
        }
      }
    }
    return retVal;
  }

} // namespace Zera

