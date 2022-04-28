#ifndef I2CMUXER_H
#define I2CMUXER_H

#include <QString>
#include <QSharedPointer>

// Although it might work for others, we use NXPs PCA9547 Muxer

class I2cMuxer
{
public:
    typedef QSharedPointer<I2cMuxer> Ptr;
    I2cMuxer(QString deviceNode, ushort i2cMuxAdress, uchar muxCodeEnableChannel, uchar muxCodeDisable);
    void enableMuxChannel();
    void disableMux();
private:
    void switchMux(uchar muxCode);
    QString m_deviceNode;
    ushort m_i2cMuxAdress;
    uchar m_muxCodeEnableChannel;
    uchar m_muxCodeDisable;
};

#endif // I2CMUXER_H
