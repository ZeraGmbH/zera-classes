#include <syslog.h>
#include <crcutils.h>
#include <QString>

#include "zera_mcontroller_base.h"
#include "i2cutils.h"

// we have this at many many places -> TODO: rework and move to a common place
#define DEBUG1 (m_nDebugLevel & 1) // log all error messages
#define DEBUG2 (m_nDebugLevel & 2) // log all i2c transfers
//#define DEBUG3 (m_nDebugLevel & 4) // log all client connect/disconnects

ZeraMcontrollerBase::ZeraMcontrollerBase(QString devnode, quint8 adr, quint8 debuglevel)
    : m_pCRCGenerator(new cMaxim1WireCRC()),
      m_sI2CDevNode(devnode),
      m_nI2CAdr(adr),
      m_nDebugLevel(debuglevel),
      m_nLastErrorFlags(0)
{
}

ZeraMcontrollerBase::~ZeraMcontrollerBase()
{
    delete m_pCRCGenerator;
}

ZeraMcontrollerBase::atmelRM ZeraMcontrollerBase::startProgram()
{
    atmelRM ret;
    struct bl_cmd blStartProgramCMD(BL_CMD_START_PROGRAM, nullptr, 0);
    if ( writeBootloaderCommand(&blStartProgramCMD)!= 0 || m_nLastErrorFlags!=0 ) {
        ret = cmdexecfault;
    }
    else {
        ret = cmddone;
    }
    return ret;
}

ZeraMcontrollerBase::atmelRM ZeraMcontrollerBase::loadFlash(cIntelHexFileIO &ihxFIO)
{
    return loadMemory(BL_CMD_WRITE_FLASH_BLOCK, ihxFIO);
}

ZeraMcontrollerBase::atmelRM ZeraMcontrollerBase::loadEEprom(cIntelHexFileIO &ihxFIO)
{
    return loadMemory(BL_CMD_WRITE_EEPROM_BLOCK, ihxFIO);
}

ZeraMcontrollerBase::atmelRM ZeraMcontrollerBase::readVariableLenText(quint16 hwcmd, QString& answer)
{
    atmelRM ret = cmdexecfault;

    // Repsonse has variable length. So we need to get length first
    // and call readOutput below
    struct hw_cmd CMD(hwcmd, 0, nullptr, 0 );
    qint16 bytesToRead = writeCommand(&CMD);

    if ( bytesToRead > 0 && m_nLastErrorFlags==0 ) {
        char *localAnsw = new char[bytesToRead];
        qint16 bytesRead = readOutput(reinterpret_cast<quint8*>(localAnsw), static_cast<quint16>(bytesToRead));
        if (bytesRead == bytesToRead) {
            localAnsw[bytesToRead-1] = 0;
            answer = localAnsw;
            ret = cmddone;
        }
        delete [] localAnsw;
    }
    return ret;
}

qint16 ZeraMcontrollerBase::writeCommand(hw_cmd * hc, quint8 *dataReceive, quint16 dataAndCrcLen)
{
    qint16 rlen = -1; // return value ; < 0 means error
    quint8 inpBuf[5]; // the answer always has 5 bytes

    GenCommand(hc);
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
        syslog(LOG_INFO,"i2c cmd start: adr 0x%02X / cmd 0x%04X / dev 0x%02X / par %s / len %i",
               m_nI2CAdr, hc->cmdcode, hc->device, qPrintable(i2cHexParam), dataAndCrcLen);
    }

    int errVal = I2CTransfer(m_sI2CDevNode, m_nI2CAdr, m_nDebugLevel, &comData);
    if (!errVal) { // if no error
        // Checksum OK?
        quint8 expectedCrc = m_pCRCGenerator->CalcBlockCRC(inpBuf, 4);
        quint8 receivedCrc = inpBuf[4];
        if (expectedCrc==receivedCrc) {
            m_nLastErrorFlags |= static_cast<quint16>(inpBuf[0] << 8) + inpBuf[1];
            // Error mask ok?
            if (m_nLastErrorFlags == 0) {
                rlen = static_cast<qint16>((inpBuf[2] << 8) + inpBuf[3]);
                if(DEBUG2) {
                    syslog(LOG_INFO, "i2c cmd ok: adr 0x%02X / len %i",
                           m_nI2CAdr, rlen);
                }
                // Read cmd data?
                if(dataReceive && dataAndCrcLen) {
                    if(rlen == dataAndCrcLen) {
                        // readOutput logs -> no need to add logs here
                        rlen = readOutput(dataReceive, dataAndCrcLen);
                    }
                    else if(DEBUG1) {
                        rlen = -1;
                        m_nLastErrorFlags |= PC_ERR_FLAG_LENGTH;
                        syslog(LOG_ERR,"i2c cmd was: adr 0x%02X / cmd 0x%04X / dev 0x%02X / par %s / len %i",
                               m_nI2CAdr, hc->cmdcode, hc->device, qPrintable(i2cHexParam), dataAndCrcLen);
                        syslog(LOG_ERR, "i2c cmd returned wrong length: adr 0x%02X / expected len %i / received len %i",
                               m_nI2CAdr, dataAndCrcLen, rlen);
                    }
                }
            }
            else if(DEBUG1) {
                syslog(LOG_ERR,"i2c cmd was: adr 0x%02X / cmd 0x%04X / dev 0x%02X / par %s",
                       m_nI2CAdr, hc->cmdcode, hc->device, qPrintable(i2cHexParam));
                syslog(LOG_ERR, "i2c cmd error: adr 0x%02X / mask 0x%04X",
                       m_nI2CAdr, m_nLastErrorFlags);
            }
        }
        else {
            m_nLastErrorFlags |= PC_ERR_FLAG_CRC;
            if (DEBUG1) {
                syslog(LOG_ERR,"i2c cmd was: adr 0x%02X / cmd 0x%04X / dev 0x%02X / par %s",
                       m_nI2CAdr, hc->cmdcode, hc->device, qPrintable(i2cHexParam));
                syslog(LOG_ERR, "i2c cmd checksum error: adr 0x%02X / expected 0x%02X / received: 0x%02X",
                       m_nI2CAdr, expectedCrc, receivedCrc);
            }
        }
    }
    else {
        m_nLastErrorFlags |= PC_ERR_FLAG_I2C_TRANSFER;
        if (DEBUG1) {
            syslog(LOG_ERR,"i2c cmd was: adr 0x%02X / cmd 0x%04X / dev 0x%02X / par %s",
                   m_nI2CAdr, hc->cmdcode, hc->device, qPrintable(i2cHexParam));
            syslog(LOG_ERR, "i2c cmd failed: adr 0x%02X / error returned %i",
                   m_nI2CAdr, errVal);
        }
    }
    // GenCommand allocated mem for us -> cleanup
    delete hc->cmddata;
    return rlen; // return -1  on error else length info
}


qint16 ZeraMcontrollerBase::writeBootloaderCommand(bl_cmd* blc, quint8 *dataReceive, quint16 dataAndCrcLen)
{
    qint16 rlen = -1; // length of data avaliable / < 0 signals error
    quint8 inpBuf[5]; // command response's length is always 5

    GenBootloaderCommand(blc);
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
    if (DEBUG1 || DEBUG2) {
        for(iByte=0; iByte<blc->plen; iByte++) {
           i2cHexParam += QString("0x%1 ").arg(blc->par[iByte], 2, 16, QLatin1Char('0'));
        }
    }
    if (DEBUG2) {
        syslog(LOG_INFO,"i2c bootcmd start: addr 0x%02X / cmd 0x%02X / par %s / len %i",
               m_nI2CAdr, blc->cmdcode, qPrintable(i2cHexParam), dataAndCrcLen);
    }

    int errVal = I2CTransfer(m_sI2CDevNode, m_nI2CAdr, m_nDebugLevel, &comData);
    if (!errVal) { // no error?
        // Checksum OK?
        quint8 expectedCrc = m_pCRCGenerator->CalcBlockCRC(inpBuf, 4);
        quint8 receivedCrc = inpBuf[4];
        if (expectedCrc==receivedCrc) {
            m_nLastErrorFlags |= static_cast<quint16>(inpBuf[0] << 8) + inpBuf[1];
            // Error mask ok?
            if (m_nLastErrorFlags == 0) {
                rlen = static_cast<qint16>((inpBuf[2] << 8) + inpBuf[3]);
                // This is one of my favorite 'make is complicated wherever
                // you can' - and yes I was involved:
                // Bootloader and Hardware protocol interpret length differently:
                //   hardware-protocol: Len of data + crc
                //   bootloader:        Len of data only
                // Since this is a possible pitfall treat rlen same as writeCommand
                if(rlen > 0) {
                    ++rlen;
                }
                if(DEBUG2) {
                    syslog(LOG_INFO, "i2c bootcmd ok: adr 0x%02X / len %i",
                           m_nI2CAdr, rlen);
                }
                // Read cmd data?
                if(dataReceive && dataAndCrcLen) {
                    if(rlen == dataAndCrcLen) {
                        // readOutput logs -> no need to add logs here
                        rlen = readOutput(dataReceive, dataAndCrcLen);
                    }
                    else if(DEBUG1) {
                        rlen = -1;
                        m_nLastErrorFlags |= PC_ERR_FLAG_LENGTH;
                        syslog(LOG_ERR,"i2c bootcmd was: adr 0x%02X / cmd 0x%02X / par %s / len %i",
                               m_nI2CAdr, blc->cmdcode, qPrintable(i2cHexParam), dataAndCrcLen);
                        syslog(LOG_ERR, "i2c bootcmd returned wrong length: adr 0x%02X / expected len %i / received len %i",
                               m_nI2CAdr, dataAndCrcLen, rlen);
                    }
                }
            }
            else {
                m_nLastErrorFlags |= PC_ERR_FLAG_CRC;
                if(DEBUG1) {
                    syslog(LOG_ERR,"i2c bootcmd was: addr 0x%02X / cmd 0x%02X / par %s",
                           m_nI2CAdr, blc->cmdcode, qPrintable(i2cHexParam));
                    syslog(LOG_ERR, "i2c bootcmd error: adr 0x%02X / mask 0x%04X",
                           m_nI2CAdr, m_nLastErrorFlags);
                }
            }
        }
        else {
            m_nLastErrorFlags |= PC_ERR_FLAG_I2C_TRANSFER;
            if (DEBUG1) {
                syslog(LOG_ERR,"i2c bootcmd was: addr 0x%02X / cmd 0x%02X / par %s",
                       m_nI2CAdr, blc->cmdcode, qPrintable(i2cHexParam));
                syslog(LOG_ERR, "i2c bootcmd checksum error: adr 0x%02X / expected 0x%02X / received: 0x%02X",
                       m_nI2CAdr, expectedCrc, receivedCrc);
            }
        }
    }
    else if (DEBUG1) {
        syslog(LOG_ERR,"i2c bootcmd was: addr 0x%02X / cmd 0x%02X / par %s",
               m_nI2CAdr, blc->cmdcode, qPrintable(i2cHexParam));
        syslog(LOG_ERR, "i2c bootcmd failed: adr 0x%02X / error returned %i",
               m_nI2CAdr, errVal);
    }

    // GenBootloaderCommand allocated mem for us -> cleanup
    delete blc->cmddata;
    return rlen; // -1 on error / number of data bytes we can fetch
}


qint16 ZeraMcontrollerBase::readOutput(quint8 *data, quint16 dataAndCrcLen)
{
    qint16 rlen = -1; // return value ; < 0 means error
    m_nLastErrorFlags = 0;
    // Parameter check: If something is wrong error is caused by a poor
    // implementor -> generate warning always
    if(!data) {
        syslog(LOG_WARNING, "ZeraMcontrollerBase::readOutput: No data set for return buffer");
        return rlen;
    }
    if(dataAndCrcLen < 2) {
        syslog(LOG_WARNING, "ZeraMcontrollerBase::readOutput: Minimum buffer len is 2 (1 data + 1 crc): len %i",
               dataAndCrcLen);
        return rlen;
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
        syslog(LOG_INFO,"i2c read start: adr 0x%02X / len %i",
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
        quint8 expectedCrc = m_pCRCGenerator->CalcBlockCRC(data, dataAndCrcLen-1);
        quint8 receivedCrc = data[dataAndCrcLen-1];
        if (expectedCrc==receivedCrc) {
            rlen = static_cast<qint16>(dataAndCrcLen);
            if(DEBUG2) {
                syslog(LOG_INFO, "i2c read ok: adr 0x%02X / len %i / data %s",
                       m_nI2CAdr, rlen, qPrintable(i2cHexData));
            }
        }
        else if (DEBUG1) {
            syslog(LOG_ERR, "i2c read checksum error: adr 0x%02X / expected 0x%02X / received: 0x%02X / data %s",
                   m_nI2CAdr, expectedCrc, receivedCrc, qPrintable(i2cHexData));
        }
    }
    else {
        m_nLastErrorFlags |= PC_ERR_FLAG_I2C_TRANSFER;
        if (DEBUG1) {
            syslog(LOG_ERR, "i2c read failed: adr 0x%02X / error returned %i",
                   m_nI2CAdr, errVal);
        }
    }
    return rlen; // return -1  on error else length info
}

quint32 ZeraMcontrollerBase::getLastErrorMask()
{
    return m_nLastErrorFlags;
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
    *p = m_pCRCGenerator->CalcBlockCRC(hc->cmddata, static_cast<quint32>(len-1));
    // keep cmd total len
    hc->cmdlen = len;
}


void ZeraMcontrollerBase::GenBootloaderCommand(bl_cmd* blc)
{
    if(!blc->par && blc->plen > 0) {
        syslog(LOG_WARNING, "ZeraMcontrollerBase::GenBootloaderCommand: No parameter data set but plen=%u", blc->plen);
        blc->plen = 0;
    }
    quint16 len = 4 + blc->plen;
    quint8* p = new quint8[len];
    blc->cmddata = p;

    // cmd code
    *p++ = blc->cmdcode;
    // cmd len
    *p++ = blc->plen >> 8;
    *p++ = blc->plen & 0xFF;
    // parameter
    quint8* ppar = blc->par;
    if(ppar) {
        for (int i = 0; i < blc->plen; i++) {
            *p++ = *ppar++;
        }
    }
    // CRC
    *p = m_pCRCGenerator->CalcBlockCRC(blc->cmddata, len-1);
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


ZeraMcontrollerBase::atmelRM ZeraMcontrollerBase::loadMemory(quint8 blwriteCmd, cIntelHexFileIO& ihxFIO)
{
    atmelRM  ret = cmddone;
    // Just in case there is nothing to write we have nothing to do
    if(ihxFIO.isEmpty()) {
        return ret;
    }
    // Get bootloader info (configuration)
    // Note: Response has variable length. So we need to get length first
    // and call readOutput below
    struct bl_cmd blInfoCMD(BL_CMD_READ_INFO, nullptr, 0);
    qint16 dataAndCrcLen = writeBootloaderCommand(&blInfoCMD);
    if ( dataAndCrcLen > 6 && m_nLastErrorFlags == 0 ) { // we must get at least 7 bytes
        // we've got them and no error
        quint8 blInput[255];
        qint16 read = readOutput(blInput, static_cast<quint16>(dataAndCrcLen));
        if ( read == dataAndCrcLen) { // we got the reqired information from bootloader
            blInfo BootloaderInfo;
            quint8* dest = reinterpret_cast<quint8*>(&BootloaderInfo);
            // Note: Bootloader response starts with zero-terminated version
            // information. Swap-copy bytes following into BootloaderInfo
            size_t pos = strlen(reinterpret_cast<char*>(blInput));
            size_t i;
            for (i = 0; i < 4; i++) {
                dest[i ^ 1] = blInput[pos+1+i]; // little endian ... big endian
            }
            dest[i] = blInput[pos+1+i];

            quint32 MemAdress = 0;
            quint32 MemOffset;
            QByteArray MemByteArray;

            ihxFIO.GetMemoryBlock( BootloaderInfo.MemPageSize, MemAdress, MemByteArray, MemOffset);
            while ( (MemByteArray.count()) && (ret == cmddone) ) { // as long we get data from hexfile
                // Set address pointer
                quint8* adrParameter;
                quint8 adrParLen = BootloaderInfo.AdressPointerSize;
                adrParameter = GenAdressPointerParameter(adrParLen, MemAdress);
                struct bl_cmd blAdressCMD(BL_CMD_WRITE_ADDRESS_POINTER, adrParameter, adrParLen);
                if ( writeBootloaderCommand(&blAdressCMD) == 0 && m_nLastErrorFlags == 0 ) {
                    // write data
                    quint8* memdat = reinterpret_cast<quint8*>(MemByteArray.data());
                    quint16 memlen = static_cast<quint16>(MemByteArray.count());
                    struct bl_cmd blwriteMemCMD(blwriteCmd, memdat, memlen);
                    if ( writeBootloaderCommand(&blwriteMemCMD) == 0 && m_nLastErrorFlags == 0 ) {
                        // we were able to write the data and expect the data to be in flash when sent over i2c
                        MemAdress += BootloaderInfo.MemPageSize;
                        // try to get next block from hex file
                        ihxFIO.GetMemoryBlock( BootloaderInfo.MemPageSize, MemAdress, MemByteArray, MemOffset);
                    }
                    else {
                        ret = cmdexecfault;
                    }
                }
                else {
                    ret = cmdexecfault;
                }
                delete adrParameter;
            }
        }
        else {
            ret = cmdexecfault;
        }
    }
    else {
        ret = cmdexecfault;
    }

    return ret;
}
