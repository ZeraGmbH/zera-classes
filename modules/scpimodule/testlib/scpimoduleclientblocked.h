#ifndef SCPIMODULECLIENTBLOCKED_H
#define SCPIMODULECLIENTBLOCKED_H

#include <QTcpSocket>
#include <QObject>
#include <QByteArrayList>


class ScpiModuleClientBlocked : public QObject
{
    Q_OBJECT
public:
    explicit ScpiModuleClientBlocked(QString ipAddress = "127.0.0.1", int port = 6320);
    QByteArray sendReceive(QByteArray send, bool removeLineFeedOnReceive = true);
    void sendMulti(QByteArrayList send);
    QByteArrayList receiveMulti();
    void closeSocket();

private:
    QTcpSocket m_socket;
};

#endif // SCPIMODULECLIENTBLOCKED_H
