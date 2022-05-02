#ifndef PROTOCOL_ZERA_BOOTLOADER_H
#define PROTOCOL_ZERA_BOOTLOADER_H

/**
 * ZERA bootloader protocol definitions for µCs. For mor details see [1]
 * [1] smb://s-zera-stor01/data/EntwHard/Libraries/Atmel%20AVR/Docs/Bootloader.doc
 */

#include "zeradev_export.h"
#include <QtCore/qglobal.h>

/**
 * @brief Bootloader command codes
 */
constexpr quint8 BL_CMD_READ_INFO = 0x00;
constexpr quint8 BL_CMD_START_PROGRAM = 0x01;
constexpr quint8 BL_CMD_READ_FLASH_BLOCK = 0x10;
constexpr quint8 BL_CMD_READ_EEPROM_BLOCK = 0x11;
constexpr quint8 BL_CMD_READ_ADDRESS_POINTER = 0x12;
constexpr quint8 BL_CMD_WRITE_FLASH_BLOCK = 0x20;
constexpr quint8 BL_CMD_WRITE_EEPROM_BLOCK = 0x21;
constexpr quint8 BL_CMD_WRITE_ADDRESS_POINTER = 0x22;


/**
 * @brief bootloader error flag definitions
 */
constexpr quint16 BL_ERR_FLAG_BUFFER_OVERFLOW = 1<<0;
constexpr quint16 BL_ERR_FLAG_HEX_INVALID = 1<<1;
constexpr quint16 BL_ERR_FLAG_LENGTH = 1<<2;
constexpr quint16 BL_ERR_FLAG_CRC = 1<<3;
constexpr quint16 BL_ERR_FLAG_CMD_INVALID = 1<<4;
constexpr quint16 BL_ERR_FLAG_ADR_RANGE = 1<<5;
constexpr quint16 BL_ERR_FLAG_EXECUTE = 1<<6;


/**
 * @brief Bootloader command structure
 */
struct bl_cmd
{
    bl_cmd(quint8 _cmdcode, quint8* _par, quint16 _paramOrRequestedLen)
        : cmdlen(0), cmddata(nullptr) {
        cmdcode = _cmdcode;
        par = _par;
        paramOrRequestedLen = _paramOrRequestedLen;
    }
    quint8 cmdcode;
    quint8* par;
    quint16 paramOrRequestedLen;
    quint16 cmdlen;
    quint8* cmddata;
};

/**
 * @brief Configuration flag bit numbers reported by Read Info (ID 0x00) command
 */
enum enConfFlags {
    blAutoIncr = 0, blReadCommandsAvail = 1
};


/**
 * @brief Bootloader info configuration flags
 */
constexpr quint16 BL_CONFIG_FLAG_AUTO_INCREMENT = 1<<0;
constexpr quint16 BL_CONFIG_FLAG_READ_CMDS_SUPPORTED = 1<<1;

/**
 * @brief Bootloader configuration info structure reported by Read Info (ID 0x00) command
 */
struct blInfo {
    quint16 ConfigurationFlags;
    quint16 MemPageSize;
    uchar AdressPointerSize;
};

#endif // PROTOCOL_ZERA_BOOTLOADER_H
