#ifndef ABSTRACTSERVERINTERFACE_H
#define ABSTRACTSERVERINTERFACE_H

#include <QAbstractSocket>
#include <QVariant>

class AbstractServerInterface : public QObject
{
    Q_OBJECT
signals:
    void tcpError(QAbstractSocket::SocketError errorCode);
    void serverAnswer(quint32 msgnr, quint8 reply, QVariant answer);
};

typedef std::shared_ptr<AbstractServerInterface> AbstractServerInterfacePtr;

#endif // ABSTRACTSERVERINTERFACE_H
