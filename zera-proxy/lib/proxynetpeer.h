#ifndef PROXYNETPEER_H
#define PROXYNETPEER_H

#include <xiqnetpeer.h>

namespace Zera { namespace Proxy {

class cProxyNetPeer: public XiQNetPeer
{
    Q_OBJECT
public:
    cProxyNetPeer(QObject *qObjParent = 0);
    void startProxyConnection(QString ipAddress, quint16 port);
    bool isStarted();
private:
    bool m_bStarted;
};

}}
#endif // PROXYNETPEER_H
