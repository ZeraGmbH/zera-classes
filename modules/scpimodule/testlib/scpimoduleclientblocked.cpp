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

QByteArrayList ScpiModuleClientBlocked::sendReceiveMulti(QByteArrayList send)
{
    QElapsedTimer timer;
    timer.start();

    for(auto &item : send)
        m_socket.write(item + "\n");

    TimeMachineObject::feedEventLoop();

    QByteArray received = m_socket.readAll();
    QByteArrayList ret = received.split('\n');
    qInfo("Network I/O received: %s / took %llims", qPrintable(received), timer.elapsed());
    return ret;
}

