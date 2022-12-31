#ifndef I2CMUXER_H
#define I2CMUXER_H

#include "i2cmuxerinterface.h"
#include <QString>

// Although it might work for others, we use NXPs PCA9547 Muxer

class I2cMuxer : public I2cMuxerInterface
{
public:
    I2cMuxer(QString deviceNode, ushort i2cMuxAdress, uchar muxCodeEnableChannel, uchar muxCodeDisable);
    void enableMuxChannel() override;
    void disableMux() override;
    virtual QString getDevIdString() override;
private:
    void switchMux(uchar muxCode);
    QString m_deviceNode;
    ushort m_i2cMuxAdress;
    uchar m_muxCodeEnableChannel;
    uchar m_muxCodeDisable;
};

#endif // I2CMUXER_H
