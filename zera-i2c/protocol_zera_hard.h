#ifndef PROTOCOL_ZERA_HARD_H
#define PROTOCOL_ZERA_HARD_H

/**
 * ZERA hardware protocol definitions for µCs. For more details see [1]
 * [1] smb://s-zera-stor01/data/EntwHard/Libraries/Atmel%20AVR/Docs/CommunicationProtocols.doc
 */

#include "dev_global.h"

/**
 * @brief hardware command error flag definitions
 */
constexpr quint16 HW_ERR_FLAG_CRC = 1<<0;
constexpr quint16 HW_ERR_FLAG_LENGTH = 1<<1;
constexpr quint16 HW_ERR_CMD_FOR_DEV_NOT_AVAIL = 1<<2;
constexpr quint16 HW_ERR_FLAG_EXECUTE = 1<<3;
constexpr quint16 HW_ERR_LAYER2 = 1<<4;
constexpr quint16 HW_ERR_PARAM_INVALID = 1<<5;
constexpr quint16 HW_ERR_ASCII_HEX = 1<<6;


/**
 * @brief ZERA µC command structure
 */
struct hw_cmd {
    hw_cmd(quint16 _cmdcode, quint8 _device, quint8* _par, quint16 _plen)
        : cmdlen(0), cmddata(nullptr) {
        cmdcode = _cmdcode;
        device = _device;
        par = _par;
        plen = _plen;
    }
    /**
     * @brief cmdcode: Command identifying number
     */
    quint16 cmdcode;
    /**
     * @brief device: subdevice number
     */
    quint8 device;
    /**
     * @brief par: paramter data
     */
    quint8* par;
    /**
     * @brief plen: parameter length
     */
    quint16 plen;
    /**
     * @brief cmdlen: total length of command
     */
    quint16 cmdlen;
    /**
     * @brief cmddata: raw data send
     */
    quint8* cmddata;
};

#endif // PROTOCOL_ZERA_HARD_H
