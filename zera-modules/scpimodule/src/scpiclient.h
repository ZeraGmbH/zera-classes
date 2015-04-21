#ifndef SCPICLIENT_H
#define SCPICLIENT_H

#include <QObject>

class QTcpSocket;

namespace SCPIMODULE
{

class cSCPIInterface;

class cSCPIClient: public QObject
{
    Q_OBJECT

 public:
    cSCPIClient(QTcpSocket* socket, cSCPIInterface* iface);
    ~cSCPIClient();

    void setAuthorisation(bool auth);

private:
    QTcpSocket* m_pSocket;
    cSCPIInterface* m_pSCPIInterface;
    bool m_bAuthorisation;

private slots:
    void cmdInput();
    void receiveStatus(quint8 stat);
    void receiveAnswer(QString answ);

};

}
#endif // SCPICLIENT_H
