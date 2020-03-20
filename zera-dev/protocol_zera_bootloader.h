#ifndef PROTOCOL_ZERA_BOOTLOADER_H
#define PROTOCOL_ZERA_BOOTLOADER_H

/**
 * ZERA bootloader protocol definitions for µCs. For mor details see [1]
 * [1] smb://s-zera-stor01/data/EntwHard/Libraries/Atmel%20AVR/Docs/Bootloader.doc
 */

#include "dev_global.h"

/**
 * @brief Bootloader command codes
 */
enum bl_cmdcode
{
    blReadInfo = 0x00,
    blStartProgram = 0x01,
    blReadFlashBlock = 0x10,
    blReadEEPromBlock = 0x11,
    blReadAdressPointer = 0x12,
    blWriteFlashBlock = 0x20,
    blWriteEEPromBlock = 0x21,
    blWriteAddressPointer = 0x22
};

/**
 * @brief Bootloader command structure
 */
struct bl_cmd
{
    bl_cmd(quint8 _cmdcode, quint8* _par, quint16 _plen)
        : cmdlen(0), cmddata(nullptr), RM(0){
        cmdcode = _cmdcode;
        par = _par;
        plen = _plen;
    }
    quint8 cmdcode;
    quint8* par;
    quint16 plen;
    quint16 cmdlen;
    quint8* cmddata;
    quint16 RM;
};

/**
 * @brief Configuration flag bit numbers reported by Read Info (ID 0x00) command
 */
enum enConfFlags {
    blAutoIncr = 0, blReadCommandsAvail = 1
};

/**
 * @brief Bootloader configuration info structure reported by Read Info (ID 0x00) command
 */
struct blInfo {
    quint16 ConfigurationFlags;
    quint16 MemPageSize;
    uchar AdressPointerSize;
};

#endif // PROTOCOL_ZERA_BOOTLOADER_H
