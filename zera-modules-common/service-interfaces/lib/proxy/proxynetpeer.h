#ifndef PROXYNETPEER_H
#define PROXYNETPEER_H

#include <xiqnetpeer.h>

namespace Zera { namespace Proxy {

class ProxyNetPeer: public XiQNetPeer
{
    Q_OBJECT
public:
    ProxyNetPeer(QObject *qObjParent = 0);
    void startProxyConnection(QString ipAddress, quint16 port);
    bool isStarted();
private:
    bool m_bStarted;
};

}}
#endif // PROXYNETPEER_H
