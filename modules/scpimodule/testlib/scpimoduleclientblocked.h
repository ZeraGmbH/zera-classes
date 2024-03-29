#ifndef SCPIMODULECLIENTBLOCKED_H
#define SCPIMODULECLIENTBLOCKED_H

#include <QTcpSocket>
#include <QObject>

class ScpiModuleClientBlocked : public QObject
{
    Q_OBJECT
public:
    explicit ScpiModuleClientBlocked(QString ipAddress = "127.0.0.1", int port = 6320);
    QString sendReceive(QByteArray send, bool removeLineFeedOnReceive = true);

private:
    QTcpSocket m_socket;
};

#endif // SCPIMODULECLIENTBLOCKED_H
