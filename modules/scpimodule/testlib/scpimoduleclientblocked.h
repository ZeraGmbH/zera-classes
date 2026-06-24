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
    void setLogFile(const QString &logFileName);

    QByteArray sendReceive(const QByteArray &send, bool removeLineFeedOnReceive = true);
    void sendMulti(const QByteArrayList &send);
    QByteArrayList receiveMulti();

    void closeSocket();

private:
    QTcpSocket m_socket;
    QString m_logFileName;
    enum LogDirection {
        LOG_SEND,
        LOG_RECEIVE
    };
    void addLog(LogDirection direction, const QByteArray &logData);
};

#endif // SCPIMODULECLIENTBLOCKED_H
