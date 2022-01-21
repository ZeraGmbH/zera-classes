#include "iodevicezeraserial.h"

#include <QSerialPortAsyncBlock>


IoDeviceZeraSerial::IoDeviceZeraSerial(IoDeviceTypes type) :
    IoDeviceBase(type)
{
    connect(&m_serialIO, &QSerialPortAsyncBlock::ioFinished, this, &IoDeviceZeraSerial::onIoFinished);
    connect(&m_disappearWatcher, &FileDisappearWatcher::sigFileRemoved,
            this, &IoDeviceZeraSerial::onDeviceFileGone, Qt::QueuedConnection);
    // TBD: we need a vein logger
    //m_serialIO.enableDebugMessages(true);
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
    setReadTimeoutNextIoSerial(timeoutMs, sourceDefaultMsBetweenTwoBytes);
}

IoDeviceZeraSerial::~IoDeviceZeraSerial()
{
    _close();
}

int IoDeviceZeraSerial::sendAndReceive(IoTransferDataSingle::Ptr ioTransferData)
{
    prepareSendAndReceive(ioTransferData);
    if(m_serialIO.isOpen()) {
        // set read timeout
        const TTimeoutParam* timeoutParam = &defaultTimeoutParam;
        if(nextTimeoutWasSet) {
            timeoutParam = &nextTimeoutParam;
            nextTimeoutWasSet = false;
        }
        m_serialIO.setReadTimeout(timeoutParam->iMsReceiveFirst, timeoutParam->iMsBetweenTwoBytes, timeoutParam->iMsMinTotal);
        // block end criteria
        const TBlockEndCriteria* endCriteria = &defaultBlockEndCriteria;
        if(nextBlockEndCriteriaWasSet) {
            endCriteria = &nextBlockEndCriteria;
            nextBlockEndCriteriaWasSet = false;
        }
        m_serialIO.setBlockEndCriteria(endCriteria->iBlockLenReceive, endCriteria->endBlock);

        // try io
        m_serialIO.sendAndReceive(ioTransferData->getByesSend(), &m_dataReceived);
    }
    if(!m_serialIO.isIOPending()) {
        emit _sigIoFinished(m_currIoId.getCurrent(), true);
    }
    return m_currIoId.getCurrent();
}

void IoDeviceZeraSerial::setReadTimeoutNextIoSerial(int iMsReceiveFirst, int iMsBetweenTwoBytes, int iMsMinTotal)
{
    nextTimeoutParam.iMsReceiveFirst = iMsReceiveFirst;
    nextTimeoutParam.iMsBetweenTwoBytes = iMsBetweenTwoBytes;
    nextTimeoutParam.iMsMinTotal = iMsMinTotal;
    nextTimeoutWasSet = true;
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
    m_ioTransferData->setDataReceived(m_dataReceived);
    emit _sigIoFinished(m_currIoId.getCurrent(), false);
}

void IoDeviceZeraSerial::onDeviceFileGone(QString)
{
    close();
    emit sigDisconnected();
}

void IoDeviceZeraSerial::_close()
{
    if(m_serialIO.isOpen()) {
        m_serialIO.close();
    }
    m_disappearWatcher.resetFiles();
}
