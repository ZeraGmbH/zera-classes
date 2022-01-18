#include "iodevicebaseserial.h"

IODeviceBaseSerial::IODeviceBaseSerial(QObject *parent) : QObject(parent)
{
    connect(this, &IODeviceBaseSerial::sigIoFinishedToQueue, this, &IODeviceBaseSerial::sigIoFinished, Qt::QueuedConnection);
}

IODeviceBaseSerial::~IODeviceBaseSerial()
{
}

int IODeviceBaseSerial::sendAndReceive(QByteArray, QByteArray*)
{
    m_currIoId.setCurrent(m_IDGenerator.nextID());
    emit sigIoFinishedToQueue(m_currIoId.getCurrent(), true);
    return m_currIoId.getCurrent();
}

QString IODeviceBaseSerial::getDeviceInfo()
{
    return m_strDeviceInfo;
}
