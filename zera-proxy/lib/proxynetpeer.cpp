#include "proxynetpeer.h"

namespace Zera { namespace Proxy {

cProxyNetPeer::cProxyNetPeer(QObject *qObjParent)
    :XiQNetPeer(qObjParent)
{
    m_bStarted = false;
}

void cProxyNetPeer::startProxyConnection(QString ipAddress, quint16 port)
{
    startConnection(ipAddress, port);
    m_bStarted = true;
}

bool cProxyNetPeer::isStarted()
{
    return m_bStarted;
}

}}
