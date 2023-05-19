#include "zeramcontrolleriotemplate.h"

ZeraMControllerIoTemplate::ZeraMControllerIoTemplate(QString devnode, quint8 adr, quint8 debuglevel) :
    m_sI2CDevNode(devnode),
    m_nI2CAdr(adr),
    m_nDebugLevel(debuglevel)
{
}
