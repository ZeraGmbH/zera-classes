#ifndef ZERA_MCONTROLLER_BASE_H
#define ZERA_MCONTROLLER_BASE_H

/**
 * ZERA hardware protocol definitions for µCs. For mor details see [1]
 * [1] smb://s-zera-stor01/data/EntwHard/Libraries/Atmel%20AVR/Docs/CommunicationProtocols.doc
 */

#include "zera-i2c-devices_export.h"
#include "protocol_zera_bootloader.h"
#include "protocol_zera_hard.h"
#include "zera_mcontroller_errorflags.h"
#include <QString>
#include <crcutils.h>
#include <intelhexfileio.h>

/**
 * @brief ZeraMcontrollerBase implements basic functionality for hardware-/bootloader-protocol
 */
class ZERA_I2C_DEVICES_EXPORT ZeraMcontrollerBase
{
public:
    /**
     * @brief Command response as handled in this scope
     */
    enum atmelRM
    {
        cmddone,
        cmdfault,
        cmdexecfault
    };
    /**
     * @brief ZeraMcontrollerBase: constructor
     * @param devnode: full path to i2c block-device
     * @param adr: i2c addres of slave
     * @param debuglevel: control verbosity of log output
     */
    ZeraMcontrollerBase(QString devnode, quint8 adr, quint8 debuglevel);
    /**
     * @brief setMaxWriteMemRetry: Set maximum writes in case of auto-verify
     * @param _maxBlockWriteTries: Max block writes in case of auto-verify errors (FWIW: 0 will run 256 times!!)
     */
    void setMaxWriteMemRetry(quint8 _maxBlockWriteTries);
    /* bootloader is common to all controllers and should never change for
     * compatibility -> complete interface to control bootloader
     * can be implemented in base class
     */
    /**
     * @brief startProgram: Bootloader command to start application
     * @return
     */
    atmelRM startProgram();
    atmelRM loadFlash(cIntelHexFileIO& ihxFIO);
    atmelRM loadEEprom(cIntelHexFileIO& ihxFIO);
    atmelRM verifyFlash(cIntelHexFileIO& ihxFIO);
    atmelRM verifyEEprom(cIntelHexFileIO& ihxFIO);
    /**
     * @brief readVariableLenText: Function to read strings with variable length (name,version..) from controller
     * @param hwcmd: [in] command id
     * @param answer: [out] String read from controller
     * @return done or error type information
     */
    atmelRM readVariableLenText(quint16 hwcmd, QString& answer);
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
    /**
     * @brief GenCommand: Allocate & fill raw data to send in cmddata
     * @param hc: pointer to command struct
     */
    void GenCommand(hw_cmd* hc);
    /**
     * @brief GenBootloaderCommand: Allocate & fill raw data to send in cmddata
     * @param blc: pointer to command struct
     */
    void GenBootloaderCommand(bl_cmd* blc);
    /**
     * @brief GenAdressPointerParameter: Helper allocating and setting address pointer array used by loadMemory
     * @param adresspointerSize: size of address in bytes
     * @param adr: The addres to calculate array from
     * @return: pointer to created array
     */
    quint8* GenAdressPointerParameter(quint8 adresspointerSize, quint32 adr);
    /**
     * @brief loadMemory: Let bootloader write Intel hex/eep file to controller
     * @param blWriteCmd: write cmd id: either BL_CMD_WRITE_FLASH_BLOCK or BL_CMD_WRITE_EEPROM_BLOCK
     * @param blReadCmd: read cmd id: either BL_CMD_READ_FLASH_BLOCK or BL_CMD_READ_EEPROM_BLOCK
     * @param ihxFIO: Intel hex file object
     * @return done or error type information
     */
    atmelRM loadOrVerifyMemory(quint8 blCmd, cIntelHexFileIO& ihxFIO, bool verify);

    QString m_sI2CDevNode;
    quint8 m_nI2CAdr;
    quint8 m_nDebugLevel;
    quint32 m_nLastErrorFlags;
    bool m_bBootCmd;
    quint8 maxBlockWriteTries;
};


#endif // ZERA_MCONTROLLER_BASE_H
