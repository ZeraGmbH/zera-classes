#ifndef H2013_ZERANET_CLIENT_H
#define H2013_ZERANET_CLIENT_H

#include <QObject>
#include <QStateMachine>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
class QFinalState;
class QTimer;
QT_END_NAMESPACE

namespace Zera
{
  namespace Net
  {
    class _ClientPrivate : public QStateMachine
    {
      Q_OBJECT
    public:
      explicit _ClientPrivate(quint32 socketDescriptor, QString clientName = QString(), QObject *parent = 0);

      /**
        @b returns the name of the client (something like RMS or Oscilloscope)
        */
      const QString &getName();
      /**
        @b returns the socket descriptor of the clients socket
        */
      int getSocket();
      /**
        @b Reads a QString from the socket
        */
      QByteArray readClient();

    signals:
      void clientConnected();
      void clientDisconnected();
      void clientLogout();
      void sockError(QAbstractSocket::SocketError socketError);
      void messageReceived(QByteArray message);

    public slots:
      /**
       * @brief Tell the client to disconnect
       */
      void logoutClient();
      void setName(QString newName);
      /**
        @b Writes a QString to the socket
        */
      void writeClient(QByteArray message);

    private slots:
      void initialize();
      void maintainConnection();
      void disconnectClient();

    private:
      void setupStateMachine();
      /**
        @b The actual socket of the Server::Client
        */
      QTcpSocket* clSocket;

      QString name;
      const quint32 sockDescriptor;

      QFinalState *fstDisconnected;
      QState *stAboutToDisconnect;
      QState *stConnected;
      QState *stContainer;
      QState *stInit;






      Q_DISABLE_COPY(_ClientPrivate)
    };
  }
}
#endif // H2013_ZERANET_CLIENT_H
