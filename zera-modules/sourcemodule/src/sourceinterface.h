#ifndef SOURCEINTERFACEBASE_H
#define SOURCEINTERFACEBASE_H

#include <QObject>

class cSourceInterfaceTransactionIdGenerator
{
public:
    /**
     * @brief nextTransactionID
     * @return unique value != 0
     */
    int nextTransactionID();
private:
    int m_currentTransActionID = -1;
};

/**
 * @brief cSourceInterfaceBase: Interface + make sure ioFinished is queued
 */
class cSourceInterfaceBase : public QObject
{
    Q_OBJECT
public:
    explicit cSourceInterfaceBase(QObject *parent = nullptr);
    /**
     * @brief sendAndReceive
     * @param dataSend
     * @param pDataReceive
     * @return != 0: transactionID / = 0: transactionID not started
     */
    virtual int sendAndReceive(QByteArray dataSend, QByteArray* pDataReceive);
signals:
    void ioFinished(int transactionID); // users connect this signal
    void ioFinishedToQueue(int transactionID); // sub classes emit this to ensure queue

protected:
    cSourceInterfaceTransactionIdGenerator m_transactionIDGenerator;
};


/**
 * @brief cSourceInterfaceDemo: Emit ioFinished
 */
class cSourceInterfaceDemo : public cSourceInterfaceBase
{
    Q_OBJECT
public:
    virtual int sendAndReceive(QByteArray dataSend, QByteArray* pDataReceive) override;
};

#endif // SOURCEINTERFACEBASE_H
