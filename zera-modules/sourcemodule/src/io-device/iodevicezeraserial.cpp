#include "iodevicezeraserial.h"

#include "filedisappearwatcher.h"
#include <QSerialPortAsyncBlock>

static constexpr int sourceDefaultMsBetweenTwoBytes = 500;

class IoDeviceZeraSerialPrivate
{
public:
    IoDeviceZeraSerialPrivate() {}

    QSerialPortAsyncBlock m_serialIO;

    struct TTimeoutParam
    {
        int iMsReceiveFirst = sourceDefaultTimeout;
        int iMsBetweenTwoBytes = sourceDefaultMsBetweenTwoBytes;
        int iMsMinTotal = 0;
    };
    const TTimeoutParam defaultTimeoutParam;
    TTimeoutParam nextTimeoutParam;
    bool nextTimeoutWasSet = false;

    struct TBlockEndCriteria
    {
        int iBlockLenReceive = 0;
        QByteArray endBlock = QByteArray("\r");
    };
    const TBlockEndCriteria defaultBlockEndCriteria;
    TBlockEndCriteria nextBlockEndCriteria;
    bool nextBlockEndCriteriaWasSet = false;

    cFileDisappearWatcher m_disappearWatcher;
};

IoDeviceZeraSerial::IoDeviceZeraSerial(QObject *parent) :
    IODeviceBaseSerial(parent),
    d_ptr(new IoDeviceZeraSerialPrivate())
{
    connect(&d_ptr->m_serialIO, &QSerialPortAsyncBlock::ioFinished, this, &IoDeviceZeraSerial::onIoFinished);
    connect(&d_ptr->m_disappearWatcher, &cFileDisappearWatcher::sigFileRemoved,
            this, &IoDeviceZeraSerial::onDeviceFileGone, Qt::QueuedConnection);
    // TBD: we need a vein logger
    d_ptr->m_serialIO.enableDebugMessages(true);
}

bool IoDeviceZeraSerial::open(QString strDeviceInfo)
{
    bool open = false;
    QStringList splitList = strDeviceInfo.split("/", Qt::SkipEmptyParts);
    if(splitList.count() > 0) {
        QString portName = splitList.last();
        d_ptr->m_serialIO.setPortName(portName);
        // hard code settings for now
        d_ptr->m_serialIO.setBaudRate(9600);
        d_ptr->m_serialIO.setDataBits(QSerialPort::Data8);
        d_ptr->m_serialIO.setParity(QSerialPort::NoParity);
        d_ptr->m_serialIO.setStopBits(QSerialPort::TwoStop);
        open = d_ptr->m_serialIO.open(QIODevice::ReadWrite);
        if(open) {
            m_strDeviceInfo = strDeviceInfo;
            d_ptr->m_disappearWatcher.watchFile(strDeviceInfo);
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
    setReadTimeoutNextIo(timeoutMs, sourceDefaultMsBetweenTwoBytes);
}

IoDeviceZeraSerial::~IoDeviceZeraSerial()
{
    _close();
}

int IoDeviceZeraSerial::sendAndReceive(QByteArray bytesSend, QByteArray *pDataReceive)
{
    m_currIoId.setCurrent(m_IDGenerator.nextID());
    if(d_ptr->m_serialIO.isOpen()) {
        // set read timeout
        const IoDeviceZeraSerialPrivate::TTimeoutParam* timeoutParam = &d_ptr->defaultTimeoutParam;
        if(d_ptr->nextTimeoutWasSet) {
            timeoutParam = &d_ptr->nextTimeoutParam;
            d_ptr->nextTimeoutWasSet = false;
        }
        d_ptr->m_serialIO.setReadTimeout(timeoutParam->iMsReceiveFirst, timeoutParam->iMsBetweenTwoBytes, timeoutParam->iMsMinTotal);
        // block end criteria
        const IoDeviceZeraSerialPrivate::TBlockEndCriteria* endCriteria = &d_ptr->defaultBlockEndCriteria;
        if(d_ptr->nextBlockEndCriteriaWasSet) {
            endCriteria = &d_ptr->nextBlockEndCriteria;
            d_ptr->nextBlockEndCriteriaWasSet = false;
        }
        d_ptr->m_serialIO.setBlockEndCriteria(endCriteria->iBlockLenReceive, endCriteria->endBlock);

        // try io
        d_ptr->m_serialIO.sendAndReceive(bytesSend, pDataReceive);
    }
    if(!d_ptr->m_serialIO.isIOPending()) {
        emit sigIoFinishedToQueue(m_currIoId.getCurrent(), true);
    }
    return m_currIoId.getCurrent();
}

void IoDeviceZeraSerial::setReadTimeoutNextIo(int iMsReceiveFirst, int iMsBetweenTwoBytes, int iMsMinTotal)
{
    d_ptr->nextTimeoutParam.iMsReceiveFirst = iMsReceiveFirst;
    d_ptr->nextTimeoutParam.iMsBetweenTwoBytes = iMsBetweenTwoBytes;
    d_ptr->nextTimeoutParam.iMsMinTotal = iMsMinTotal;
    d_ptr->nextTimeoutWasSet = true;
}

void IoDeviceZeraSerial::setBlockEndCriteriaNextIo(int iBlockLenReceive, QByteArray endBlock)
{
    d_ptr->nextBlockEndCriteria.iBlockLenReceive = iBlockLenReceive;
    d_ptr->nextBlockEndCriteria.endBlock = endBlock;
    d_ptr->nextBlockEndCriteriaWasSet = true;
}

bool IoDeviceZeraSerial::isOpen()
{
    return d_ptr->m_serialIO.isOpen();
}

void IoDeviceZeraSerial::onIoFinished()
{
    emit sigIoFinishedToQueue(m_currIoId.getCurrent(), false);
}

void IoDeviceZeraSerial::onDeviceFileGone(QString)
{
    close();
    emit sigDisconnected();
}

void IoDeviceZeraSerial::_close()
{
    if(d_ptr->m_serialIO.isOpen()) {
        d_ptr->m_serialIO.close();
    }
    d_ptr->m_disappearWatcher.resetFiles();
}
