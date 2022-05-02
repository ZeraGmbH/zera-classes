#include <i2cutils.h>
#include <i2cmuxer.h>

I2cMuxer::I2cMuxer(QString deviceNode, ushort i2cMuxAdress, uchar muxCodeEnableChannel, uchar muxCodeDisable) :
    m_deviceNode(deviceNode),
    m_i2cMuxAdress(i2cMuxAdress),
    m_muxCodeEnableChannel(muxCodeEnableChannel),
    m_muxCodeDisable(muxCodeDisable)
{
}

void I2cMuxer::enableMuxChannel()
{
    switchMux(m_muxCodeEnableChannel);
}

void I2cMuxer::disableMux()
{
    switchMux(m_muxCodeDisable);
}

void I2cMuxer::switchMux(uchar muxCode)
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
