#ifndef I2CMUXERPCA9547_H
#define I2CMUXERPCA9547_H

#include "i2cmuxerinterface.h"

class I2cMuxerPCA9547 : public I2cMuxerInterface
{
public:
    I2cMuxerPCA9547(QString deviceNode, ushort i2cMuxAdress, int channel0to7);
    void enableMuxChannel() override;
    void disableMux() override;
    virtual QString getDevIdString() override;
private:
    void switchMux(uchar muxCode);
    QString m_deviceNode;
    ushort m_i2cMuxAdress;
    int m_channel0to7;
};

#endif // I2CMUXERPCA9547_H
