#include "groupvalidator.h"
#include "commandvalidator.h"

#include <QEvent>
#include <ve_commandevent.h>
#include <ve_eventdata.h>

namespace Zera
{

  GroupValidator::GroupValidator(QObject *t_parent) : VeinEvent::EventSystem(t_parent)
  {

  }

  void GroupValidator::insertEntityValidator(int t_entityId, CommandValidator *t_cValidator)
  {
    m_entityValidators.insert(t_entityId, t_cValidator);
  }

  void GroupValidator::removeEntityValidator(int t_entityId)
  {
    m_entityValidators.remove(t_entityId);
  }

  const QHash<int, CommandValidator *> GroupValidator::getEntityValidatorsCopy() const
  {
    return m_entityValidators;
  }

  void GroupValidator::setGeneralValidators(const QList<CommandValidator *> &t_vList)
  {
    m_generalValidators = t_vList;
  }

  bool GroupValidator::processEvent(QEvent *t_event)
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

            for(int i=0; i<m_generalValidators.count() && cEvent->isAccepted() == false; ++i)
            {
              m_generalValidators.at(i)->processCommandEvent(cEvent);
            }

            if(m_entityValidators.contains(cEvent->eventData()->entityId()))
            {
              m_entityValidators.value(cEvent->eventData()->entityId())->processCommandEvent(cEvent);
            }


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
