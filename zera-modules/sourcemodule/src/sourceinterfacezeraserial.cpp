#include "sourceinterface.h"

class cSourceInterfaceZeraSerialPrivate
{
public:
    cSourceInterfaceZeraSerialPrivate() {}

    struct TTimeoutParam
    {
        int iMsReceiveFirst = 1000;
        int iMsBetweenTwoBytes = 500;
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
};

cSourceInterfaceZeraSerial::cSourceInterfaceZeraSerial(QObject *parent) :
    cSourceInterfaceBase(parent),
    d_ptr(new cSourceInterfaceZeraSerialPrivate())
{
    connect(&m_serialIO, &QSerialPortAsyncBlock::ioFinished, this, &cSourceInterfaceZeraSerial::onIoFinished);
    // TBD: we need a vein logger
    m_serialIO.enableDebugMessages(true);
}

bool cSourceInterfaceZeraSerial::open(QString strDeviceInfo)
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
    }
    return open;
}

void cSourceInterfaceZeraSerial::close()
{
    m_serialIO.close();
}

int cSourceInterfaceZeraSerial::sendAndReceive(QByteArray dataSend, QByteArray *pDataReceive)
{
    // set read timeout
    const cSourceInterfaceZeraSerialPrivate::TTimeoutParam* timeoutParam = &d_ptr->defaultTimeoutParam;
    if(d_ptr->nextTimeoutWasSet) {
        timeoutParam = &d_ptr->nextTimeoutParam;
        d_ptr->nextTimeoutWasSet = false;
    }
    m_serialIO.setReadTimeout(timeoutParam->iMsReceiveFirst, timeoutParam->iMsBetweenTwoBytes, timeoutParam->iMsMinTotal);
    // block end criteria
    const cSourceInterfaceZeraSerialPrivate::TBlockEndCriteria* endCriteria = &d_ptr->defaultBlockEndCriteria;
    if(d_ptr->nextBlockEndCriteriaWasSet) {
        endCriteria = &d_ptr->nextBlockEndCriteria;
        d_ptr->nextBlockEndCriteriaWasSet = false;
    }
    m_serialIO.setBlockEndCriteria(endCriteria->iBlockLenReceive, endCriteria->endBlock);

    // try io
    m_serialIO.sendAndReceive(dataSend, pDataReceive);
    if(m_serialIO.isIOPending()) { // ZERA: One transaction at a time -> no hash for m_currentTransactionID
        m_currentTransactionID = m_transactionIDGenerator.nextTransactionID();
    }
    else {
        m_currentTransactionID = 0;
    }
    return m_currentTransactionID;
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

void cSourceInterfaceZeraSerial::onIoFinished()
{
    emit ioFinishedToQueue(m_currentTransactionID);
}
