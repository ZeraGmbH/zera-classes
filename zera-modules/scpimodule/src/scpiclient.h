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
    virtual ~cSCPIClient();

    void setAuthorisation(bool auth);

public slots:
    void receiveStatus(quint8 stat);
    void receiveAnswer(QString answ);

private:
    QTcpSocket* m_pSocket;
    cSCPIInterface* m_pSCPIInterface;
    bool m_bAuthorisation;

private slots:
    void cmdInput();

};

}
#endif // SCPICLIENT_H
