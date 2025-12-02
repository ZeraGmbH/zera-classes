#include "scpimoduleclientblocked.h"
#include <timemachineobject.h>
#include <testloghelpers.h>
#include <QElapsedTimer>
#include <QFile>

ScpiModuleClientBlocked::ScpiModuleClientBlocked(QString ipAddress, int port)
{
    m_socket.connectToHost(ipAddress, port);
    TimeMachineObject::feedEventLoop();
}

void ScpiModuleClientBlocked::setLogFile(const QString &logFileName)
{
    m_logFileName = logFileName;
    TestLogHelpers::writeFile(m_logFileName, ""); // more a write with parent dirs
}

QByteArray ScpiModuleClientBlocked::sendReceive(const QByteArray &send, bool removeLineFeedOnReceive)
{
    QElapsedTimer timer;
    timer.start();

    addLog(LOG_SEND, send);
    m_socket.write(send + "\n");
    TimeMachineObject::feedEventLoop();

    QByteArray ret = m_socket.readAll();
    if(!ret.isEmpty()) {
        addLog(LOG_RECEIVE, ret);
        if(removeLineFeedOnReceive)
            ret.replace("\n", "");
        qInfo("Network I/O received: %s / took %llims", qPrintable(ret), timer.elapsed());
    }
    else
        qInfo("No respone received");
    return ret;
}

void ScpiModuleClientBlocked::sendMulti(const QByteArrayList &send)
{
    for(const auto &item : send) {
        QByteArray line = item + "\n";
        addLog(LOG_SEND, line);
        m_socket.write(line);
    }
}

QByteArrayList ScpiModuleClientBlocked::receiveMulti()
{
    QByteArray receive = m_socket.readAll();
    addLog(LOG_RECEIVE, receive);
    if(receive.endsWith('\n'))
        receive = receive.left(receive.length() - 1);
    return receive.split('\n');
}

void ScpiModuleClientBlocked::closeSocket()
{
    m_socket.close();
}

void ScpiModuleClientBlocked::addLog(LogDirection direction, const QByteArray &logData)
{
    if (m_logFileName.isEmpty())
        return;

    QByteArray linePrefix;
    switch(direction) {
    case LOG_SEND:
        linePrefix = "Send:    ";
        break;
    case LOG_RECEIVE:
        linePrefix = "Receive: ";
        break;
    }
    QByteArray out = logData;
    if (!out.endsWith("\n"))
        out+="\n";
    QFile file(m_logFileName);
    if(file.open(QFile::WriteOnly | QFile::Append))
        file.write(linePrefix + out);
}
