#ifndef RMCONNECTION_H
#define RMCONNECTION_H

#include <xiqnetwrapper.h>
#include <xiqnetpeer.h>
#include <netmessages.pb.h>
#include <QString>
#include <QAbstractSocket>

class XiQNetPeer;

class cRMConnection: public QObject
{
    Q_OBJECT
public:
    cRMConnection(QString ipadr, quint16 port, quint8 dlevel);
    void connect2RM();
    void SendIdent(QString ident);
public slots:
    void SendCommand(QString& cmd, QString &par, quint32 msgnr);
private:
    QString m_sIPAdr;
    quint16 m_nPort;
    quint8 m_nDebugLevel;
    QString m_sCommand;
    XiQNetPeer* m_pResourceManagerClient;
    XiQNetWrapper m_ProtobufWrapper;
private slots:
    void tcpErrorHandler(QAbstractSocket::SocketError errorCode);
    void responseHandler(std::shared_ptr<google::protobuf::Message> response);
signals:
    void connectionRMError();
    void connected();
    void rmAck(quint32);
};

#endif // RMCONNECTION_H
