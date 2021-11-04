#include "sourceinterface.h"

int cSourceInterfaceTransactionIdGenerator::nextTransactionID()
{
    if(++m_currentTransActionID == 0) {
        ++m_currentTransActionID;
    }
    return m_currentTransActionID;
}

cSourceInterfaceBase::cSourceInterfaceBase(QObject *parent) : QObject(parent)
{
    connect(this, &cSourceInterfaceBase::ioFinishedToQueue, this, &cSourceInterfaceBase::ioFinished, Qt::QueuedConnection);
}

int cSourceInterfaceBase::sendAndReceive(QByteArray, QByteArray*)
{
    return 0;
}

int cSourceInterfaceDemo::sendAndReceive(QByteArray, QByteArray*)
{
    int transactionID = m_transactionIDGenerator.nextTransactionID();
    emit ioFinishedToQueue(transactionID);
    return transactionID;
}
