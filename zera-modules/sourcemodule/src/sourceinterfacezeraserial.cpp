#include "sourceinterface.h"
#include "filedisappearwatcher.h"

static constexpr int sourceDefaultMsBetweenTwoBytes = 500;

class cSourceInterfaceZeraSerialPrivate
{
public:
    cSourceInterfaceZeraSerialPrivate() {}

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

cSourceInterfaceZeraSerial::cSourceInterfaceZeraSerial(QObject *parent) :
    cSourceInterfaceBase(parent),
    d_ptr(new cSourceInterfaceZeraSerialPrivate())
{
    connect(&d_ptr->m_serialIO, &QSerialPortAsyncBlock::ioFinished, this, &cSourceInterfaceZeraSerial::onIoFinished);
    connect(&d_ptr->m_disappearWatcher, &cFileDisappearWatcher::sigFileRemoved,
            this, &cSourceInterfaceZeraSerial::onDeviceFileGone, Qt::QueuedConnection);
    // TBD: we need a vein logger
    d_ptr->m_serialIO.enableDebugMessages(true);
}

bool cSourceInterfaceZeraSerial::open(QString strDeviceInfo)
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
            d_ptr->m_disappearWatcher.watchFile(strDeviceInfo);
        }
    }
    return open;
}

void cSourceInterfaceZeraSerial::close()
{
    if(d_ptr->m_serialIO.isOpen()) {
        d_ptr->m_serialIO.close();
    }
    d_ptr->m_disappearWatcher.resetFiles();
}

void cSourceInterfaceZeraSerial::setReadTimeoutNextIo(int timeoutMs)
{
    setReadTimeoutNextIo(timeoutMs, sourceDefaultMsBetweenTwoBytes);
}

cSourceInterfaceZeraSerial::~cSourceInterfaceZeraSerial()
{
    close();
}

int cSourceInterfaceZeraSerial::sendAndReceive(QByteArray bytesSend, QByteArray *pDataReceive)
{
    m_currentIoID = m_IDGenerator.nextID();
    if(d_ptr->m_serialIO.isOpen()) {
        // set read timeout
        const cSourceInterfaceZeraSerialPrivate::TTimeoutParam* timeoutParam = &d_ptr->defaultTimeoutParam;
        if(d_ptr->nextTimeoutWasSet) {
            timeoutParam = &d_ptr->nextTimeoutParam;
            d_ptr->nextTimeoutWasSet = false;
        }
        d_ptr->m_serialIO.setReadTimeout(timeoutParam->iMsReceiveFirst, timeoutParam->iMsBetweenTwoBytes, timeoutParam->iMsMinTotal);
        // block end criteria
        const cSourceInterfaceZeraSerialPrivate::TBlockEndCriteria* endCriteria = &d_ptr->defaultBlockEndCriteria;
        if(d_ptr->nextBlockEndCriteriaWasSet) {
            endCriteria = &d_ptr->nextBlockEndCriteria;
            d_ptr->nextBlockEndCriteriaWasSet = false;
        }
        d_ptr->m_serialIO.setBlockEndCriteria(endCriteria->iBlockLenReceive, endCriteria->endBlock);

        // try io
        d_ptr->m_serialIO.sendAndReceive(bytesSend, pDataReceive);
    }
    if(!d_ptr->m_serialIO.isIOPending()) {
        emit sigIoFinishedToQueue(m_currentIoID, true);
    }
    return m_currentIoID;
}

void cSourceInterfaceZeraSerial::setReadTimeoutNextIo(int iMsReceiveFirst, int iMsBetweenTwoBytes, int iMsMinTotal)
{
    d_ptr->nextTimeoutParam.iMsReceiveFirst = iMsReceiveFirst;
    d_ptr->nextTimeoutParam.iMsBetweenTwoBytes = iMsBetweenTwoBytes;
    d_ptr->nextTimeoutParam.iMsMinTotal = iMsMinTotal;
    d_ptr->nextTimeoutWasSet = true;
}

void cSourceInterfaceZeraSerial::setBlockEndCriteriaNextIo(int iBlockLenReceive, QByteArray endBlock)
{
    d_ptr->nextBlockEndCriteria.iBlockLenReceive = iBlockLenReceive;
    d_ptr->nextBlockEndCriteria.endBlock = endBlock;
    d_ptr->nextBlockEndCriteriaWasSet = true;
}

bool cSourceInterfaceZeraSerial::isOpen()
{
    return d_ptr->m_serialIO.isOpen();
}

void cSourceInterfaceZeraSerial::onIoFinished()
{
    emit sigIoFinishedToQueue(m_currentIoID, false);
}

void cSourceInterfaceZeraSerial::onDeviceFileGone(QString)
{
    close();
    emit sigDisconnected();
}
