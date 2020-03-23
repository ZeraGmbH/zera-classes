#ifndef PROTOCOL_ZERA_HARD_H
#define PROTOCOL_ZERA_HARD_H

/**
 * ZERA hardware protocol definitions for µCs. For more details see [1]
 * [1] smb://s-zera-stor01/data/EntwHard/Libraries/Atmel%20AVR/Docs/CommunicationProtocols.doc
 */

#include "dev_global.h"

/**
 * @brief ZERA µC command structure
 */
struct hw_cmd {
    hw_cmd(quint16 _cmdcode, quint8 _device, quint8* _par, quint16 _plen)
        : cmdlen(0), cmddata(nullptr), RM(0)
    {
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
    /**
     * @brief RM: error mask of response
     */
    quint16 RM;
};

#endif // PROTOCOL_ZERA_HARD_H
