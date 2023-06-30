#include "iodevicezeraserial.h"
#include <QSerialPortAsyncBlock>

IoDeviceZeraSerial::IoDeviceZeraSerial(IoDeviceTypes type) :
    IoDeviceBase(type)
{
    connect(&m_serialIO, &QSerialPortAsyncBlock::ioFinished, this, &IoDeviceZeraSerial::onIoFinished);
    connect(&m_disappearWatcher, &FileDisappearWatcher::sigFileRemoved,
            this, &IoDeviceZeraSerial::onDeviceFileGone);
}

IoDeviceZeraSerial::~IoDeviceZeraSerial()
{
    _close();
}

bool IoDeviceZeraSerial::open(QString strDeviceInfo)
{
    bool open = false;
    QStringList splitList = strDeviceInfo.split("/", Qt::SkipEmptyParts);
    if(splitList.count() > 0) {
        QString portName = splitList.last();
        m_serialIO.setPortName(portName);
        // hard code settings for now
        m_serialIO.setBaudRate(9600);
        m_serialIO.setDataBits(QSerialPort::Data8);
        m_serialIO.setParity(QSerialPort::NoParity);
        m_serialIO.setStopBits(QSerialPort::TwoStop);
        open = m_serialIO.open(QIODevice::ReadWrite);
        if(open) {
            m_strDeviceInfo = strDeviceInfo;
            m_disappearWatcher.watchFile(strDeviceInfo);
        }
    }
    return open;
}

void IoDeviceZeraSerial::close()
{
    _close();
}

void IoDeviceZeraSerial::setReadTimeoutNextIo(int timeoutMs)
{
    setReadTimeoutNextIoSerial(timeoutMs != 0 ? timeoutMs : m_defaultTimeoutMs, serialIoDefaultMsBetweenTwoBytes);
}

int IoDeviceZeraSerial::sendAndReceive(IoTransferDataSingle::Ptr ioTransferData)
{
    prepareSendAndReceive(ioTransferData);
    if(m_serialIO.isOpen()) {
        setReadTimeout();
        setBlockEndCriteria();
        m_serialIO.sendAndReceive(ioTransferData->getBytesSend(), &m_bytesReceived);
    }
    if(!m_serialIO.isIOPending()) {
        emit sigIoFinishedQueued(m_currIoId.getPending(), true);
    }
    return m_currIoId.getPending();
}

void IoDeviceZeraSerial::setDefaultTimeout(int defaultTimeoutMs)
{
    m_defaultTimeoutMs = defaultTimeoutMs;
}

void IoDeviceZeraSerial::setReadTimeoutNextIoSerial(int iMsReceiveFirst, int iMsBetweenTwoBytes, int iMsMinTotal)
{
    nextTimeoutParam.iMsReceiveFirst = iMsReceiveFirst;
    nextTimeoutParam.iMsBetweenTwoBytes = iMsBetweenTwoBytes;
    nextTimeoutParam.iMsMinTotal = iMsMinTotal;
    nextTimeoutWasSet = true;
}

void IoDeviceZeraSerial::setReadTimeout()
{
    const TTimeoutParam* timeoutParam = &defaultTimeoutParam;
    if(nextTimeoutWasSet) {
        timeoutParam = &nextTimeoutParam;
        nextTimeoutWasSet = false;
    }
    m_serialIO.setReadTimeout(timeoutParam->iMsReceiveFirst, timeoutParam->iMsBetweenTwoBytes, timeoutParam->iMsMinTotal);
}

void IoDeviceZeraSerial::setBlockEndCriteria()
{
    const TBlockEndCriteria* endCriteria = &defaultBlockEndCriteria;
    if(nextBlockEndCriteriaWasSet) {
        endCriteria = &nextBlockEndCriteria;
        nextBlockEndCriteriaWasSet = false;
    }
    m_serialIO.setBlockEndCriteria(endCriteria->iBlockLenReceive, endCriteria->endBlock);
}

void IoDeviceZeraSerial::setBlockEndCriteriaNextIo(int iBlockLenReceive, QByteArray endBlock)
{
    nextBlockEndCriteria.iBlockLenReceive = iBlockLenReceive;
    nextBlockEndCriteria.endBlock = endBlock;
    nextBlockEndCriteriaWasSet = true;
}

bool IoDeviceZeraSerial::isOpen()
{
    return m_serialIO.isOpen();
}

void IoDeviceZeraSerial::onIoFinished()
{
    m_ioTransferData->setDataReceived(m_bytesReceived);
    emit sigIoFinishedQueued(m_currIoId.getPending(), false);
}

void IoDeviceZeraSerial::onDeviceFileGone(QString)
{
    _close();
}

void IoDeviceZeraSerial::_close()
{
    if(m_serialIO.isOpen()) {
        m_disappearWatcher.resetFiles();
        m_serialIO.close();
        emit sigDisconnected();
    }
}
