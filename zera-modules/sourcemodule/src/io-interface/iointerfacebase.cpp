#include "iointerfacebase.h"
#include "iointerfacedemo.h"
#include "iointerfacezeraserial.h"

IoInterfaceBase::IoInterfaceBase(QObject *parent) : QObject(parent)
{
    connect(this, &IoInterfaceBase::sigIoFinishedToQueue, this, &IoInterfaceBase::sigIoFinished, Qt::QueuedConnection);
}

IoInterfaceBase::~IoInterfaceBase()
{
}

int IoInterfaceBase::sendAndReceive(QByteArray, QByteArray*)
{
    m_currIoId.setCurrent(m_IDGenerator.nextID());
    emit sigIoFinishedToQueue(m_currIoId.getCurrent(), true);
    return m_currIoId.getCurrent();
}

QString IoInterfaceBase::getDeviceInfo()
{
    return m_strDeviceInfo;
}
