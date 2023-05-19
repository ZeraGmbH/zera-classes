#ifndef ZERA_MCONTROLLER_IO_H
#define ZERA_MCONTROLLER_IO_H

/**
 * ZERA hardware protocol definitions for µCs. For mor details see [1]
 * [1] smb://s-zera-stor01/data/EntwHard/Libraries/Atmel%20AVR/Docs/CommunicationProtocols.doc
 */

#include "zeramcontrolleriotemplate.h"
#include "protocol_zera_bootloader.h"
#include "protocol_zera_hard.h"
#include "zera_mcontroller_errorflags.h"
#include <QString>
#include <crcutils.h>
#include <intelhexfileio.h>

class ZERA_I2C_DEVICES_EXPORT ZeraMControllerIo : public ZeraMControllerIoTemplate
{
public:
    ZeraMControllerIo(QString devnode, quint8 adr, quint8 debuglevel);
    /**
     * @brief setMaxWriteMemRetry: Set maximum writes in case of auto-verify
     * @param _maxBlockWriteTries: Max block writes in case of auto-verify errors (FWIW: 0 will run 256 times!!)
     */
    void setMaxWriteMemRetry(quint8 _maxBlockWriteTries);
    /* bootloader is common to all controllers and should never change for
     * compatibility -> complete interface to control bootloader
     * can be implemented in base class
     */
    atmelRM bootloaderStartProgram() override;
    atmelRM bootloaderLoadFlash(cIntelHexFileIO& ihxFIO);
    atmelRM bootloaderLoadEEprom(cIntelHexFileIO& ihxFIO);
    atmelRM bootloaderVerifyFlash(cIntelHexFileIO& ihxFIO);
    atmelRM bootloaderVerifyEEprom(cIntelHexFileIO& ihxFIO);

    atmelRM readVariableLenText(quint16 hwcmd, QString& answer) override;
    /**
     * @brief writeCommand: Write command and receive command response
     * @param hc: pointer to command struct
     * @param dataReceive: Buffer to receive data + crc
     * @param dataAndCrcLen: Expected Number of bytes to read (data + crc)
     * @return number of bytes (data+crc) we can fetch as result data
     */
    quint16 writeCommand(hw_cmd* hc, quint8 *dataReceive=nullptr, quint16 dataAndCrcLen=0);
    /**
     * @brief writeBootloaderCommand: Write bootloader command and receive command response
     * @param blc: pointer to bootloader command struct
     * @param dataReceive: Buffer to receive data + crc
     * @param dataAndCrcLen: number of bytes to read (data + crc / == 0 do not read data)
     * @return number of bytes (data+crc) dataAndCrcLen==0 we can fetch / dataAndCrcLen!=0 we have fetched
     */
    quint16 writeBootloaderCommand(bl_cmd* blc, quint8 *dataReceive=nullptr, quint16 dataAndCrcLen=0);
    /**
     * @brief readOutput: Read command result from controller
     * @param data: Buffer to receive data + crc
     * @param dataAndCrcLen: Number of bytes to read (data + crc / == 0 do not read data)
     * @return number of bytes (data+crc) dataAndCrcLen==0 we can fetch / dataAndCrcLen!=0 we have fetched
     */
    quint16 readOutput(quint8 *data, quint16 dataAndCrcLen);
    /**
     * @brief getLastErrorMask: Get error mask of last command performed
     * @return error mask
     */
    quint32 getLastErrorMask();
    /**
     * @brief getErrorMaskText: Split error mask and create a loggable string
     * @return the log string
     */
    QString getErrorMaskText();
private:
    void GenCommand(hw_cmd* hc);
    void GenBootloaderCommand(bl_cmd* blc);
    quint8* GenAdressPointerParameter(quint8 adresspointerSize, quint32 adr);
    atmelRM loadOrVerifyMemory(quint8 blCmd, cIntelHexFileIO& ihxFIO, bool verify);

    quint32 m_nLastErrorFlags;
    bool m_bBootCmd;
    quint8 maxBlockWriteTries;
};

#endif // ZERA_MCONTROLLER_IO_H
