#include "i2cmuxerpca9547.h"
#include <i2cutils.h>

I2cMuxerPCA9547::I2cMuxerPCA9547(QString deviceNode, ushort i2cMuxAdress, int channel0to7) :
    m_deviceNode(deviceNode),
    m_i2cMuxAdress(i2cMuxAdress),
    m_channel0to7(channel0to7)
{
    if(m_channel0to7 < 0 || m_channel0to7 > 7) {
        qWarning("Invalid channel on PCA9547: %i - fallback to channel 0!", m_channel0to7);
        m_channel0to7 = 0;
    }
}

void I2cMuxerPCA9547::enableMuxChannel()
{
    constexpr int muxEnableVal = (1<<3);
    switchMux(m_channel0to7 | muxEnableVal);
}

void I2cMuxerPCA9547::disableMux()
{
    switchMux(0);
}

QString I2cMuxerPCA9547::getDevIdString()
{
    return QString("%1%2").arg(m_deviceNode).arg(m_i2cMuxAdress);
}

void I2cMuxerPCA9547::switchMux(uchar muxCode)
{
    // 1 adr byte, 1 byte data = mux code
    struct i2c_msg i2cMsgs;
    i2cMsgs.addr = m_i2cMuxAdress;
    i2cMsgs.flags = 0;
    i2cMsgs.len = 1;
    i2cMsgs.buf = &muxCode;

    struct i2c_rdwr_ioctl_data i2cRdWrData;
    i2cRdWrData.msgs = &i2cMsgs;
    i2cRdWrData.nmsgs = 1;
    I2CTransfer(m_deviceNode, m_i2cMuxAdress, &i2cRdWrData);
}
