#ifndef SCPICLIENT_H
#define SCPICLIENT_H

#include <QObject>
#include <QList>

class QTcpSocket;

namespace SCPIMODULE
{

class cSCPIInterface;
class cSCPIStatus;

class cSCPIClient: public QObject
{
    Q_OBJECT

public:
    cSCPIClient(QTcpSocket* socket, cSCPIInterface* iface);
    virtual ~cSCPIClient();

    void setAuthorisation(bool auth);
    cSCPIStatus* getSCPIStatus(quint8 index);

public slots:
    void receiveStatus(quint8 stat);
    void receiveAnswer(QString answ);

private:
    QTcpSocket* m_pSocket;
    cSCPIInterface* m_pSCPIInterface;
    QList<cSCPIStatus*> m_SCPIStatusList;

    bool m_bAuthorisation;

private slots:
    void cmdInput();

};

}
#endif // SCPICLIENT_H
