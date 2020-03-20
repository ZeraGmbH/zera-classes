#ifndef ZERA_MCONTROLLER_BASE_H
#define ZERA_MCONTROLLER_BASE_H

/**
 * ZERA hardware protocol definitions for µCs. For mor details see [1]
 * [1] smb://s-zera-stor01/data/EntwHard/Libraries/Atmel%20AVR/Docs/CommunicationProtocols.doc
 */

#include <QString>
#include <crcutils.h>
#include <intelhexfileio.h>

#include "dev_global.h"
#include "protocol_zera_bootloader.h"
#include "protocol_zera_hard.h"

/**
 * @brief ZeraMcontrollerBase implements basic functionality for hardware-/bootloader-protocol
 */
class ZERADEV_EXPORT ZeraMcontrollerBase
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
    virtual ~ZeraMcontrollerBase();
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
    /**
     * @brief mGetText: Function to read strings (name,version..) from controller
     * @param hwcmd: [in] command id
     * @param answer: [out] String read from controller
     * @return done or error type information
     */
    atmelRM mGetText(quint16 hwcmd, QString& answer);
    /**
     * @brief writeCommand: Write command and receive command response
     * @param hc: pointer to command struct
     * @param dataReceive: Buffer to receive data + crc
     * @param dataAndCrcLenAndCrcLen: Number of bytes to read (data + crc)
     * @return -1 on error else number of bytes we can fetch as result data
     */
    qint16 writeCommand(hw_cmd* hc, quint8 *dataReceive=nullptr, quint16 dataAndCrcLenAndCrcLen=0);
    /**
     * @brief writeBootloaderCommand: Write bootloader command and receive command response
     * @param blc: pointer to bootloader command struct
     * @param dataReceive: Buffer to receive data + crc
     * @param dataAndCrcLenAndCrcLen: Number of bytes to read (data + crc)
     * @return -1 on error else number of bytes we can fetch as result data
     */
    qint16 writeBootloaderCommand(bl_cmd* blc, quint8 *dataReceive=nullptr, quint16 dataAndCrcLenAndCrcLen=0);
private:
    /**
     * @brief readOutput: Read command result from controller
     * @param data: Buffer to receive data + crc
     * @param dataAndCrcLenAndCrcLen: Number of bytes to read (data + crc)
     * @return -1 on error else number of bytes we can fetch
     */
    qint16 readOutput(quint8 *data, quint16 dataAndCrcLenAndCrcLen);
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
     * @param blwriteCmd: cmd id: either blWriteFlashBlock or blWriteEEPromBlock
     * @param ihxFIO: Intel hex file object
     * @return done or error type information
     */
    atmelRM loadMemory(bl_cmdcode blwriteCmd, cIntelHexFileIO& ihxFIO);

    cMaxim1WireCRC *m_pCRCGenerator;
    QString m_sI2CDevNode;
    quint8 m_nI2CAdr;
    quint8 m_nDebugLevel;
};


#endif // ZERA_MCONTROLLER_BASE_H
