#ifndef RMABSTRACTINTERFACE_H
#define RMABSTRACTINTERFACE_H

#include <QAbstractSocket>
#include <QVariant>
#include <memory>

class RmAbstractInterface : public QObject
{
    Q_OBJECT
public:
    virtual quint32 getResources(QString type) = 0;
signals:
    void tcpError(QAbstractSocket::SocketError errorCode);
    void serverAnswer(quint32 msgnr, quint8 reply, QVariant answer);
};

typedef std::shared_ptr<RmAbstractInterface> RmAbstractInterfacePtr;

#endif // RMABSTRACTINTERFACE_H
