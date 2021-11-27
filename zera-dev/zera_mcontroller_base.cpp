#include <syslog.h>
#include <crcutils.h>
#include <QString>
#include <QHash>

#include "zera_mcontroller_base.h"
#include "zera_mcontroller_errorflags.h"
#include "i2cutils.h"

// we have this at many many places -> TODO: rework and move to a common place
#define DEBUG1 (m_nDebugLevel & 1) // log all error messages
#define DEBUG2 (m_nDebugLevel & 2) // log all i2c transfers
//#define DEBUG3 (m_nDebugLevel & 4) // log all client connect/disconnects

ZeraMcontrollerBase::ZeraMcontrollerBase(QString devnode, quint8 adr, quint8 debuglevel)
    : m_sI2CDevNode(devnode),
      m_nI2CAdr(adr),
      m_nDebugLevel(debuglevel),
      m_nLastErrorFlags(0),
      m_bBootCmd(false),
      maxBlockWriteTries(2)
{
}

void ZeraMcontrollerBase::setMaxWriteMemRetry(quint8 _maxBlockWriteTries)
{
    maxBlockWriteTries = _maxBlockWriteTries;
}

ZeraMcontrollerBase::atmelRM ZeraMcontrollerBase::startProgram()
{
    struct bl_cmd blStartProgramCMD(BL_CMD_START_PROGRAM, nullptr, 0);
    writeBootloaderCommand(&blStartProgramCMD);
    return m_nLastErrorFlags!=0 ? cmdexecfault : cmddone;
}

ZeraMcontrollerBase::atmelRM ZeraMcontrollerBase::loadFlash(cIntelHexFileIO &ihxFIO)
{
    ZeraMcontrollerBase::atmelRM result = loadOrVerifyMemory(BL_CMD_WRITE_FLASH_BLOCK, ihxFIO, false);
    if(result != cmddone) {
        m_nLastErrorFlags |= MASTER_ERR_FLAG_FLASH_WRITE;
    }
    return result;
}

ZeraMcontrollerBase::atmelRM ZeraMcontrollerBase::loadEEprom(cIntelHexFileIO &ihxFIO)
{
    ZeraMcontrollerBase::atmelRM result = loadOrVerifyMemory(BL_CMD_WRITE_EEPROM_BLOCK, ihxFIO, false);
    if(result != cmddone) {
        m_nLastErrorFlags |= MASTER_ERR_FLAG_EERPOM_WRITE;
    }
    return result;
}

ZeraMcontrollerBase::atmelRM ZeraMcontrollerBase::verifyFlash(cIntelHexFileIO &ihxFIO)
{
    ZeraMcontrollerBase::atmelRM result = loadOrVerifyMemory(BL_CMD_READ_FLASH_BLOCK, ihxFIO, true);
    if(result != cmddone) {
        m_nLastErrorFlags |= MASTER_ERR_FLAG_FLASH_VERIFY;
    }
    return result;
}

ZeraMcontrollerBase::atmelRM ZeraMcontrollerBase::verifyEEprom(cIntelHexFileIO &ihxFIO)
{
    ZeraMcontrollerBase::atmelRM result = loadOrVerifyMemory(BL_CMD_READ_EEPROM_BLOCK, ihxFIO, true);
    if(result != cmddone) {
        m_nLastErrorFlags |= MASTER_ERR_FLAG_EERPOM_VERIFY;
    }
    return result;
}

ZeraMcontrollerBase::atmelRM ZeraMcontrollerBase::readVariableLenText(quint16 hwcmd, QString& answer)
{
    // Repsonse has variable length. So we need to get length first
    // and call readOutput below
    struct hw_cmd CMD(hwcmd, 0, nullptr, 0 );
    quint16 bytesToRead = writeCommand(&CMD);

    if ( bytesToRead > 0 && m_nLastErrorFlags == 0 ) {
        char *localAnsw = new char[bytesToRead];
        quint16 bytesRead = readOutput(reinterpret_cast<quint8*>(localAnsw), static_cast<quint16>(bytesToRead));
        if (m_nLastErrorFlags == 0) {
            localAnsw[bytesToRead-1] = 0;
            answer = localAnsw;
        }
        delete [] localAnsw;
    }
    return m_nLastErrorFlags == 0 ? cmddone : cmdexecfault;
}

quint16 ZeraMcontrollerBase::writeCommand(hw_cmd * hc, quint8 *dataReceive, quint16 dataAndCrcLen)
{
    quint16 dataReturnAndCrcLen = 0;
    quint8 inpBuf[5]; // the answer always has 5 bytes

    GenCommand(hc);
    m_bBootCmd = false;
    m_nLastErrorFlags = 0;

    // Send command and receive response (= error + length of data available for further read)
    struct i2c_msg Msgs[2];
    // send cmd
    Msgs[0].addr = m_nI2CAdr;
    Msgs[0].flags = 0;
    Msgs[0].len = hc->cmdlen;
    Msgs[0].buf = hc->cmddata;
    // receive cmd response
    Msgs[1].addr = m_nI2CAdr;
    Msgs[1].flags = I2C_M_RD + I2C_M_NOSTART;
    Msgs[1].len = 5;
    Msgs[1].buf = inpBuf;
    // i2c transaction data
    struct i2c_rdwr_ioctl_data comData;
    comData.msgs = Msgs;
    comData.nmsgs = 2;
    // Logging data
    QString i2cHexParam;
    if (DEBUG1 || DEBUG2) {
        quint16 iByte;
        for(iByte=0; iByte<hc->plen; iByte++) {
           i2cHexParam += QString("0x%1 ").arg(hc->par[iByte], 2, 16, QLatin1Char('0'));
        }
    }
    if (DEBUG2) {
        syslog(LOG_INFO,"i2c cmd start: adr 0x%02X / cmd 0x%04X / dev 0x%02X / par %s / len %u",
               m_nI2CAdr, hc->cmdcode, hc->device, qPrintable(i2cHexParam), dataAndCrcLen);
    }

    int errVal = I2CTransfer(m_sI2CDevNode, m_nI2CAdr, m_nDebugLevel, &comData);
    if (!errVal) { // if no error
        // Checksum OK?
        quint8 expectedCrc = cMaxim1WireCRC::CalcBlockCRC(inpBuf, 4);
        quint8 receivedCrc = inpBuf[4];
        if (expectedCrc==receivedCrc) {
            m_nLastErrorFlags |= (static_cast<quint16>(inpBuf[0]) << 8) + inpBuf[1];
            // Error mask ok?
            if (m_nLastErrorFlags == 0) {
                dataReturnAndCrcLen = (static_cast<quint16>(inpBuf[2]) << 8) + inpBuf[3];
                if(DEBUG2) {
                    syslog(LOG_INFO, "i2c cmd ok: adr 0x%02X / len %u",
                           m_nI2CAdr, dataReturnAndCrcLen);
                }
                // Read cmd data?
                if(dataReceive && dataAndCrcLen) {
                    if(dataReturnAndCrcLen == dataAndCrcLen) {
                        // readOutput logs -> no need to add logs here
                        dataReturnAndCrcLen = readOutput(dataReceive, dataAndCrcLen);
                    }
                    else {
                        if(DEBUG1) {
                            syslog(LOG_ERR,"i2c cmd was: adr 0x%02X / cmd 0x%04X / dev 0x%02X / par %s / len %u",
                                   m_nI2CAdr, hc->cmdcode, hc->device, qPrintable(i2cHexParam), dataAndCrcLen);
                            syslog(LOG_ERR, "i2c cmd returned wrong length: adr 0x%02X / expected len %u / received len %u",
                                   m_nI2CAdr, dataAndCrcLen, dataReturnAndCrcLen);
                        }
                        m_nLastErrorFlags |= MASTER_ERR_FLAG_LENGTH;
                    }
                }
            }
            else if(DEBUG1) {
                syslog(LOG_ERR,"i2c cmd was: adr 0x%02X / cmd 0x%04X / dev 0x%02X / par %s",
                       m_nI2CAdr, hc->cmdcode, hc->device, qPrintable(i2cHexParam));
                syslog(LOG_ERR, "i2c cmd error: adr 0x%02X / %s",
                       m_nI2CAdr, qPrintable(getErrorMaskText()));
            }
        }
        else {
            m_nLastErrorFlags |= MASTER_ERR_FLAG_CRC;
            if (DEBUG1) {
                syslog(LOG_ERR,"i2c cmd was: adr 0x%02X / cmd 0x%04X / dev 0x%02X / par %s",
                       m_nI2CAdr, hc->cmdcode, hc->device, qPrintable(i2cHexParam));
                syslog(LOG_ERR, "i2c cmd checksum error: adr 0x%02X / expected 0x%02X / received: 0x%02X",
                       m_nI2CAdr, expectedCrc, receivedCrc);
            }
        }
    }
    else {
        m_nLastErrorFlags |= MASTER_ERR_FLAG_I2C_TRANSFER;
        if (DEBUG1) {
            syslog(LOG_ERR,"i2c cmd was: adr 0x%02X / cmd 0x%04X / dev 0x%02X / par %s",
                   m_nI2CAdr, hc->cmdcode, hc->device, qPrintable(i2cHexParam));
            syslog(LOG_ERR, "i2c cmd failed: adr 0x%02X / error returned %i",
                   m_nI2CAdr, errVal);
        }
    }
    // GenCommand allocated mem for us -> cleanup
    delete hc->cmddata;
    return dataReturnAndCrcLen;
}


quint16 ZeraMcontrollerBase::writeBootloaderCommand(bl_cmd* blc, quint8 *dataReceive, quint16 dataAndCrcLen)
{
    quint16 dataReturnAndCrcLen = 0;
    quint8 inpBuf[5]; // command response's length is always 5

    GenBootloaderCommand(blc);
    m_bBootCmd = true;
    m_nLastErrorFlags = 0;

    i2c_msg Msgs[2];
    // send cmd
    Msgs[0].addr = m_nI2CAdr;
    Msgs[0].flags = 0;
    Msgs[0].len = blc->cmdlen;
    Msgs[0].buf = blc->cmddata;
    // receive cmd response
    Msgs[1].addr = m_nI2CAdr;
    Msgs[1].flags = I2C_M_RD + I2C_M_NOSTART;
    Msgs[1].len = 5;
    Msgs[1].buf = inpBuf;
    // i2c transaction data
    struct i2c_rdwr_ioctl_data comData;
    comData.msgs = Msgs;
    comData.nmsgs = 2;
    // Logging data
    QString i2cHexParam;
    quint16 iByte;
    if (blc->par && (DEBUG1 || DEBUG2)) {
        for(iByte=0; iByte<blc->paramOrRequestedLen; iByte++) {
           i2cHexParam += QString("0x%1 ").arg(blc->par[iByte], 2, 16, QLatin1Char('0'));
        }
    }
    if (DEBUG2) {
        syslog(LOG_INFO,"i2c bootcmd start: addr 0x%02X / cmd 0x%02X / par %s / len %u",
               m_nI2CAdr, blc->cmdcode, qPrintable(i2cHexParam), dataAndCrcLen);
    }

    int errVal = I2CTransfer(m_sI2CDevNode, m_nI2CAdr, m_nDebugLevel, &comData);
    if (!errVal) { // no error?
        // Checksum OK?
        quint8 expectedCrc = cMaxim1WireCRC::CalcBlockCRC(inpBuf, 4);
        quint8 receivedCrc = inpBuf[4];
        if (expectedCrc==receivedCrc) {
            m_nLastErrorFlags |= (static_cast<quint16>(inpBuf[0]) << 8) + inpBuf[1];
            // Error mask ok?
            if (m_nLastErrorFlags == 0) {
                dataReturnAndCrcLen = (static_cast<quint16>(inpBuf[2]) << 8) + inpBuf[3];
                // This is one of my favorite 'make is complicated wherever
                // you can' - and yes I was involved:
                // Bootloader and Hardware protocol interpret length differently:
                //   hardware-protocol: Len of data + crc
                //   bootloader:        Len of data only
                // Since this is a possible pitfall treat rlen same as writeCommand
                if(dataReturnAndCrcLen > 0) {
                    ++dataReturnAndCrcLen;
                }
                if(DEBUG2) {
                    syslog(LOG_INFO, "i2c bootcmd ok: adr 0x%02X / len %u",
                           m_nI2CAdr, dataReturnAndCrcLen);
                }
                // Read cmd data?
                if(dataReceive && dataAndCrcLen) {
                    if(dataReturnAndCrcLen == dataAndCrcLen) {
                        // readOutput logs -> no need to add logs here
                        dataReturnAndCrcLen = readOutput(dataReceive, dataAndCrcLen);
                    }
                    else {
                        if(DEBUG1) {
                            syslog(LOG_ERR,"i2c bootcmd was: adr 0x%02X / cmd 0x%02X / par %s / len %u",
                                   m_nI2CAdr, blc->cmdcode, qPrintable(i2cHexParam), dataAndCrcLen);
                            syslog(LOG_ERR, "i2c bootcmd returned wrong length: adr 0x%02X / expected len %u / received len %u",
                                   m_nI2CAdr, dataAndCrcLen, dataReturnAndCrcLen);
                        }
                        m_nLastErrorFlags |= MASTER_ERR_FLAG_LENGTH;
                    }
                }
            }
            else if(DEBUG1) {
                syslog(LOG_ERR,"i2c bootcmd was: addr 0x%02X / cmd 0x%02X / par %s",
                       m_nI2CAdr, blc->cmdcode, qPrintable(i2cHexParam));
                syslog(LOG_ERR, "i2c bootcmd error: adr 0x%02X / %s",
                       m_nI2CAdr, qPrintable(getErrorMaskText()));
            }
        }
        else {
            m_nLastErrorFlags |= MASTER_ERR_FLAG_CRC;
            if (DEBUG1) {
                syslog(LOG_ERR,"i2c bootcmd was: addr 0x%02X / cmd 0x%02X / par %s",
                       m_nI2CAdr, blc->cmdcode, qPrintable(i2cHexParam));
                syslog(LOG_ERR, "i2c bootcmd checksum error: adr 0x%02X / expected 0x%02X / received: 0x%02X",
                       m_nI2CAdr, expectedCrc, receivedCrc);
            }
        }
    }
    else  {
        m_nLastErrorFlags |= MASTER_ERR_FLAG_I2C_TRANSFER;
        if (DEBUG1) {
            syslog(LOG_ERR,"i2c bootcmd was: addr 0x%02X / cmd 0x%02X / par %s",
                   m_nI2CAdr, blc->cmdcode, qPrintable(i2cHexParam));
            syslog(LOG_ERR, "i2c bootcmd failed: adr 0x%02X / error returned %i",
                   m_nI2CAdr, errVal);
        }
    }

    // GenBootloaderCommand allocated mem for us -> cleanup
    delete blc->cmddata;
    return dataReturnAndCrcLen;
}


quint16 ZeraMcontrollerBase::readOutput(quint8 *data, quint16 dataAndCrcLen)
{
    quint16 dataReturnAndCrcLen = 0;
    m_nLastErrorFlags = 0;
    // Parameter check: If something is wrong error is caused by a poor
    // implementor -> generate warning always
    if(!data) {
        m_nLastErrorFlags |= MASTER_ERR_FLAG_CODER_IS_AN_IDIOT;
        syslog(LOG_WARNING, "ZeraMcontrollerBase::readOutput: No data set for return buffer");
        return dataReturnAndCrcLen;
    }
    if(dataAndCrcLen < 2) {
        m_nLastErrorFlags |= MASTER_ERR_FLAG_CODER_IS_AN_IDIOT;
        syslog(LOG_WARNING, "ZeraMcontrollerBase::readOutput: Minimum buffer len is 2 (1 data + 1 crc): len %u",
               dataAndCrcLen);
        return dataReturnAndCrcLen;
    }
    struct i2c_msg Msg;
    // receive cmd data
    Msg.addr = m_nI2CAdr;
    Msg.flags = I2C_M_RD;
    Msg.len = dataAndCrcLen;
    Msg.buf = data;
    // i2c transaction data
    struct i2c_rdwr_ioctl_data comData;
    comData.msgs = &Msg;
    comData.nmsgs = 1;

    if(DEBUG2) {
        syslog(LOG_INFO,"i2c read start: adr 0x%02X / len %u",
                m_nI2CAdr, dataAndCrcLen);
    }
    int errVal = I2CTransfer(m_sI2CDevNode, m_nI2CAdr, m_nDebugLevel, &comData);
    if (!errVal) { // if no error
        QString i2cHexData;
        quint16 iByte;
        if (DEBUG1 || DEBUG2) {
            for(iByte=0; iByte<dataAndCrcLen; iByte++) {
               i2cHexData += QString("0x%1 ").arg(data[iByte], 2, 16, QLatin1Char('0'));
            }
        }
        // Checksum OK?
        quint8 expectedCrc = cMaxim1WireCRC::CalcBlockCRC(data, dataAndCrcLen-1);
        quint8 receivedCrc = data[dataAndCrcLen-1];
        if (expectedCrc==receivedCrc) {
            dataReturnAndCrcLen = dataAndCrcLen;
            if(DEBUG2) {
                syslog(LOG_INFO, "i2c read ok: adr 0x%02X / len %u / data %s",
                       m_nI2CAdr, dataReturnAndCrcLen, qPrintable(i2cHexData));
            }
        }
        else {
            if (DEBUG1) {
                syslog(LOG_ERR, "i2c read checksum error: adr 0x%02X / expected 0x%02X / received: 0x%02X / data %s",
                       m_nI2CAdr, expectedCrc, receivedCrc, qPrintable(i2cHexData));
            }
            m_nLastErrorFlags |= MASTER_ERR_FLAG_CRC;
        }
    }
    else {
        m_nLastErrorFlags |= MASTER_ERR_FLAG_I2C_TRANSFER;
        if (DEBUG1) {
            syslog(LOG_ERR, "i2c read failed: adr 0x%02X / error returned %i",
                   m_nI2CAdr, errVal);
        }
    }
    return dataReturnAndCrcLen;
}

void ZeraMcontrollerBase::GenCommand(hw_cmd* hc)
{
    if(!hc->par && hc->plen > 0) {
        syslog(LOG_WARNING, "ZeraMcontrollerBase::GenCommand: No parameter data set but plen=%u", hc->plen);
        hc->plen = 0;
    }
    quint16 len = 6 + hc->plen;
    quint8* p = new quint8[len];
    hc->cmddata = p;

    // cmd len
    *p++ = static_cast<quint8>(len >> 8);
    *p++ = len & 0xFF;
    // cmd code
    *p++ = ((hc->cmdcode) >> 8) & 0xFF;
    *p++ = (hc->cmdcode) & 0xFF;
    // sub device
    *p++ = hc->device;
    // parameter
    const quint8* ppar = hc->par;
    if(ppar) {
        for (int i = 0; i < hc->plen;i++) {
            *p++ = *ppar++;
        }
    }
    // CRC
    *p = cMaxim1WireCRC::CalcBlockCRC(hc->cmddata, static_cast<quint32>(len-1));
    // keep cmd total len
    hc->cmdlen = len;
}


void ZeraMcontrollerBase::GenBootloaderCommand(bl_cmd* blc)
{
    quint16 len = 4;
    if(blc->par)
        len += blc->paramOrRequestedLen;
    quint8* p = new quint8[len];
    blc->cmddata = p;

    // cmd code
    *p++ = blc->cmdcode;
    // data len
    *p++ = blc->paramOrRequestedLen >> 8;
    *p++ = blc->paramOrRequestedLen & 0xFF;
    // parameter
    quint8* ppar = blc->par;
    if(ppar) {
        for (int i = 0; i < blc->paramOrRequestedLen; i++) {
            *p++ = *ppar++;
        }
    }
    // CRC
    *p = cMaxim1WireCRC::CalcBlockCRC(blc->cmddata, len-1);
    // keep cmd total len
    blc->cmdlen = len;
}


quint8* ZeraMcontrollerBase::GenAdressPointerParameter(quint8 adresspointerSize, quint32 adr)
{
    quint8* par = new quint8(adresspointerSize);
    quint8* pptr = par;
    for (int  i = 0; i < adresspointerSize; i++) {
        *pptr++ = static_cast<quint8>((adr >> (8* ( (adresspointerSize-1) - i)) ) & 0xff);
    }
    return par;
}


ZeraMcontrollerBase::atmelRM ZeraMcontrollerBase::loadOrVerifyMemory(quint8 blCmd, cIntelHexFileIO& ihxFIO, bool verify)
{
    // Just in case there is nothing to write we have nothing to do
    if(ihxFIO.isEmpty()) {
        return cmddone;
    }
    // Get bootloader info (configuration)
    // Note: Response has variable length. So we need to get length first
    // and call readOutput below
    struct bl_cmd blInfoCMD(BL_CMD_READ_INFO, nullptr, 0);
    quint16 dataAndCrcLen = writeBootloaderCommand(&blInfoCMD);
    if ( dataAndCrcLen > 6 && m_nLastErrorFlags == 0 ) { // we must get at least 7 bytes
        // we've got them and no error
        quint8 blInput[255];
        readOutput(blInput, static_cast<quint16>(dataAndCrcLen));
        if (m_nLastErrorFlags == 0) { // we got the reqired information from bootloader
            blInfo BootloaderInfo;
            quint8* dest = reinterpret_cast<quint8*>(&BootloaderInfo);
            // Bootloader response starts with zero-terminated version information.
            size_t pos = strlen(reinterpret_cast<char*>(blInput));
            size_t i;
            // Transfer 16Bit config + 16Bit page-size to blInfo
            for (i = 0; i < 4; i++) {
                // Controller sends 16Bit values big-endian
                #if __BYTE_ORDER == __LITTLE_ENDIAN
                    dest[i ^ 1] = blInput[pos+1+i];
                #else
                    dest[i] = blInput[pos+1+i];
                #endif
            }
            dest[i] = blInput[pos+1+i];

            // In case of verify: Check if bootloader supports read commands
            bool bootloaderSupportsRead = BootloaderInfo.ConfigurationFlags & BL_CONFIG_FLAG_READ_CMDS_SUPPORTED;
            if(verify && !bootloaderSupportsRead) {
                // It's not quite correct but this is the error we would get from µC
                m_nLastErrorFlags |= BL_ERR_FLAG_CMD_INVALID;
                syslog(LOG_WARNING, "ZeraMcontrollerBase::loadOrVerifyMemory: Bootloader does not support read commands -> cannot verify");
                return cmdexecfault; // TODO remove - not necessary: block loop won't start
            }

            // Worker vars
            quint32 memAddress = 0;
            quint32 memOffset;
            QByteArray memByteArray;
            quint8 triesLeftover = maxBlockWriteTries;

            // loop all memory blocks (re-write in case of error and further writes left)
            while ( (m_nLastErrorFlags == 0 || (!verify && triesLeftover>0 && (m_nLastErrorFlags | BL_ERR_FLAG_EXECUTE) != 0)) &&
                    ihxFIO.GetMemoryBlock(BootloaderInfo.MemPageSize, memAddress, memByteArray, memOffset) ) {
                // on retry: spawn warning
                if(triesLeftover != maxBlockWriteTries) {
                    syslog(LOG_WARNING, "Bootloader reported memory at address 0x%04X was not written correctly - retry", memAddress);
                }
                --triesLeftover;
                // Reset error flags here is OK/necessary because of:
                // * we need reset in case of repetitions
                // * on first block and subsequent blocks without m_nLastErrorFlags is 0 anyway because
                //   loop stops on other errors than write + BL_ERR_FLAG_EXECUTE
                m_nLastErrorFlags = 0;
                // Set address pointer
                quint8* adrParameter;
                quint8 adrParLen = BootloaderInfo.AdressPointerSize;
                adrParameter = GenAdressPointerParameter(adrParLen, memAddress);
                struct bl_cmd blAdressCMD(BL_CMD_WRITE_ADDRESS_POINTER, adrParameter, adrParLen);
                writeBootloaderCommand(&blAdressCMD);
                if ( m_nLastErrorFlags == 0 ) {
                    quint8* memdat = reinterpret_cast<quint8*>(memByteArray.data());
                    quint16 memlen = static_cast<quint16>(memByteArray.count());
                    // write block
                    if(!verify) {
                        struct bl_cmd blWriteMemCMD(blCmd, memdat, memlen);
                        writeBootloaderCommand(&blWriteMemCMD);
                    }
                    // verify block
                    else {
                        QByteArray memByteArrayRead(memByteArray.count() + 1/*crc*/, 0);
                        quint8* memDatRead = reinterpret_cast<quint8*>(memByteArrayRead.data());
                        quint16 memLenRead = static_cast<quint16>(memByteArrayRead.count());
                        struct bl_cmd blReadMemCMD(blCmd, nullptr, memByteArray.count());
                        writeBootloaderCommand(&blReadMemCMD, memDatRead, memLenRead);
                        if ( m_nLastErrorFlags == 0 ) {
                            memByteArrayRead = memByteArrayRead.left(memlen); // remove crc
                            if(memByteArrayRead != memByteArray) {
                                m_nLastErrorFlags |= MASTER_ERR_FLAG_VERIFY_BLOCK;
                            }
                        }
                    }
                }
                // GenAdressPointerParameter allocated adrParameter for us - cleanup
                delete adrParameter;

                // no error: next block
                if ( m_nLastErrorFlags == 0 ) {
                    memAddress += BootloaderInfo.MemPageSize;
                    // reset block repetition count
                    triesLeftover = maxBlockWriteTries;
                }
            }
        }
    }
    return m_nLastErrorFlags == 0 ? cmddone : cmdexecfault;
}


quint32 ZeraMcontrollerBase::getLastErrorMask()
{
    return m_nLastErrorFlags;
}

QString ZeraMcontrollerBase::getErrorMaskText()
{
    return ZeraMcontrollerErrorFlags::getErrorMaskText(m_nLastErrorFlags, m_bBootCmd);
}

