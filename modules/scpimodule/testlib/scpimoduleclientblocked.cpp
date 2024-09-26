#include "scpimoduleclientblocked.h"
#include <timemachineobject.h>
#include <QElapsedTimer>

ScpiModuleClientBlocked::ScpiModuleClientBlocked(QString ipAddress, int port)
{
    m_socket.connectToHost(ipAddress, port);
    TimeMachineObject::feedEventLoop();
}

QString ScpiModuleClientBlocked::sendReceive(QByteArray send, bool removeLineFeedOnReceive)
{
    QElapsedTimer timer;
    timer.start();

    m_socket.write(send + "\n");
    TimeMachineObject::feedEventLoop();

    QString ret = m_socket.readAll();
    if(removeLineFeedOnReceive)
        ret.remove("\n");
    qInfo("Network I/O received: %s / took %llims", qPrintable(ret), timer.elapsed());
    return ret;
}

void ScpiModuleClientBlocked::sendMulti(QByteArrayList send)
{
    for(auto &item : send)
        m_socket.write(item + "\n");
}

QByteArrayList ScpiModuleClientBlocked::receiveMulti()
{
    QByteArray receive = m_socket.readAll();
    if(receive.endsWith('\n'))
        receive = receive.left(receive.length() - 1);
    return receive.split('\n');
}

void ScpiModuleClientBlocked::closeSocket()
{
    m_socket.close();
}


