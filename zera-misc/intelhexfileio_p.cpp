#include <QFile>
#include <QList>

#include "intelhexfileio_p.h"


THexFileMemRegion::THexFileMemRegion()
{
    nStartAddress = 0;
}


THexFileMemRegion::THexFileMemRegion(int StartAddress, const QByteArray& byteArray)
{
    nStartAddress = StartAddress;
    ByteArrContent = byteArray;
}


THexFileMemRegion& THexFileMemRegion::operator = (const THexFileMemRegion& obj)
{
    nStartAddress = obj.nStartAddress;
    ByteArrContent = obj.ByteArrContent;
    return *this;
}


bool THexFileMemRegion::operator == (const THexFileMemRegion& obj) const
{
    return (&(ByteArrContent) == &(obj.ByteArrContent));
}


quint32 THexFileMemRegion::GetMaxAddress()
{
    return nStartAddress + ByteArrContent.size() - 1;
}


THexFileMessage::THexFileMessage(ulong _nLineNo, const QString& _sMsgText)
{
    nLineNo = _nLineNo;
    sMsgText = _sMsgText;
}


enum enHexField
{
    HEX_UNDEF = 0,

    HEX_LEN,	// 1 byte len
    HEX_ADR0,	// 2 bytes address
    HEX_ADR1,
    HEX_TYP,	// 1 byte type
    HEX_DATA,	// n bytes data
    HEX_CHK	// 1 byte checksum
};


enum enRecordType
{
    RECTYPE_UNDEF = 0,

    RECTYPE_DATA,		// data record
    RECTYPE_LAST,		// end of file record
    RECTYPE_EXT_SEG,	// extended segment adress record
    RECTYPE_START_SEG,	// start segment adresss record
    RECTYPE_EXT_ADR,	// linear address record (upper 16Bit of linear address)
    RECTYPE_START_ADR	// start address
};



cIntelHexFileIOPrivate::cIntelHexFileIOPrivate()
{
    m_byteErasedByteValue = 0xff;
}

bool cIntelHexFileIOPrivate::ReadHexFile(const QString& fileName)
{
    bool readHexError = false;
    m_strFileName = fileName;
    QFile file( fileName);
    ulong nCurrInputLine = 1;

    if (file.open( QIODevice::ReadOnly))
    {
    // Reset memory contents
    m_ListMemRegions.clear();
    m_sListErrWarn.clear();

    // state/temp data keepers
    enHexField eHexField = HEX_UNDEF;
    char chHexDigit[3] = "00";
    bool bUpperHexDigit = true;
    bool bLastReceived = false;
    ulong nByteRead = 0;
        quint32 nCurrDataPos = 0;

    // for all subsequent records
    ulong nAddressExtension = 0;

    // Per record
        quint32 nCurrDataLen = 0;
        quint32 nCurrAddress = 0;
        enRecordType enCurrRecordType = RECTYPE_UNDEF;
    uchar byteCheckSum = 0;
    uchar DataBuff[256];

    do
    {
#define BUFF_LEN 1024
        char FileBuffer[BUFF_LEN];
        nByteRead = file.readLine(FileBuffer, BUFF_LEN);
        for (ulong iByte =  0; iByte < nByteRead; iByte++)
        {
        char chCurr = FileBuffer[iByte];
        // Check <LF> for lin count
        if(chCurr == '\n')
            nCurrInputLine++;
        // 1st check if we are inside of a block
        if (eHexField == HEX_UNDEF)
        {
            // Start
            if (chCurr == ':')
            {
            // There is no more record expected
            if(!bLastReceived)
            {
                // Init
                eHexField = HEX_LEN;
                bUpperHexDigit = true;
                nCurrDataLen = 0;
                nCurrAddress = 0;
                enCurrRecordType = RECTYPE_UNDEF;
                byteCheckSum = 0;
                memset(DataBuff, 0, sizeof(DataBuff));
                nCurrDataPos = 0;
            }
            else
            {
                // Error Msg

                m_sListErrWarn.append(THexFileMessage( nCurrInputLine, QString("Record found after end record\n")));
                readHexError = true;

            }
            }
        }
        else
        {
            // Within a frame all must be hex
            if (isHexText(chCurr))
            {
            // Upper nibble
            if (bUpperHexDigit)
                chHexDigit[0] = chCurr;
            // Lower nibble terminates a byte
            else
            {
                chHexDigit[1] = chCurr;
                uchar byteDecoded = Hex2Bin(chHexDigit);
                byteCheckSum += byteDecoded;
                switch(eHexField)
                {
                // Keep upper nibble
                case HEX_LEN:
                    // keep data length
                    nCurrDataLen = byteDecoded;
                    // next field
                    eHexField = HEX_ADR0;
                    break;
                case HEX_ADR0:
                    // keep upper address
                    nCurrAddress = byteDecoded << 8;
                    // next field
                    eHexField = HEX_ADR1;
                    break;
                case HEX_ADR1:
                    // keep lower address
                    nCurrAddress += byteDecoded;
                    // next field
                    eHexField = HEX_TYP;
                    break;
                case HEX_TYP:
                    // keep record type
                    switch(byteDecoded)
                    {
                          case 0:
                         enCurrRecordType = RECTYPE_DATA;
                         break;
                          case 1:
                          enCurrRecordType = RECTYPE_LAST;
                          break;
                          case 2:
                          enCurrRecordType = RECTYPE_EXT_SEG;
                          break;
                          case 3:
                          enCurrRecordType = RECTYPE_START_SEG;						      break;				                        			          case 4:
                          enCurrRecordType = RECTYPE_EXT_ADR;
                          break;
                          case 5:
                          enCurrRecordType = RECTYPE_START_ADR;
                          break;
                          // unkown record type
                         default:
                         // Error Msg
                         m_sListErrWarn.append(THexFileMessage( nCurrInputLine, QString("Record has unkwown type\n")));
                         readHexError = true;
                         eHexField = HEX_UNDEF;
                         break;
                    }

                   // next field
                   if(nCurrDataLen != 0)
                   eHexField = HEX_DATA;
                   else
                   eHexField = HEX_CHK;
                   break;

                case HEX_DATA:
                    // Data
                    DataBuff[nCurrDataPos] = byteDecoded;
                    nCurrDataPos++;
                    // next field
                    if (nCurrDataPos==nCurrDataLen)
                    eHexField = HEX_CHK;
                    break;

                case HEX_CHK:
                    // Test Checksum
                    if (byteCheckSum == 0)
                    {
                    // The action is depending on record type
                    switch (enCurrRecordType)
                    {
                        // Data
                       case RECTYPE_DATA:
                           {
                           QByteArray byteArray;
                           byteArray.resize(nCurrDataLen);
                                                   for (quint32 ByteNo = 0; ByteNo < nCurrDataLen; ByteNo++)
                               byteArray[ByteNo] = DataBuff[ByteNo];
                           m_ListMemRegions.append(THexFileMemRegion(nAddressExtension+nCurrAddress, byteArray));
                           break;
                           }

                       // Last record
                       case RECTYPE_LAST:
                           bLastReceived = true;
                           break;

                       // Extended segment
                       case RECTYPE_EXT_SEG:
                           nAddressExtension = DataBuff[0] << 12;
                           nAddressExtension += DataBuff[1] << 4;
                           break;

                      // Start segment (not supported)
                      case RECTYPE_START_SEG:
                          m_sListErrWarn.append(THexFileMessage(nCurrInputLine, QString("Record for start adress not supported\n")));
                         readHexError = true;
                         break;

                      // Linear address record (upper 16Bit of linear address)
                    case RECTYPE_EXT_ADR:
                        nAddressExtension = DataBuff[0] << 24;
                        nAddressExtension += DataBuff[1] << 16;
                        break;

                    // Start address (not supported)
                    case RECTYPE_START_ADR:
                        m_sListErrWarn.append(THexFileMessage(nCurrInputLine, QString("Record for start adress not supported\n")));
                        readHexError = true;
                        break;

                    case RECTYPE_UNDEF:
                        break;
                          }

                    }
                    else
                    {
                    // Error Msg
                    m_sListErrWarn.append(THexFileMessage(nCurrInputLine, QString("Checksum error ocurred\n")));
                    readHexError = true;
                    }

                    // terminat of record
                    eHexField = HEX_UNDEF;
                    break;

                case HEX_UNDEF:
                    break;
                }
            }

            bUpperHexDigit = !bUpperHexDigit;
            }
            else
            {
            // Error Msg
            m_sListErrWarn.append(THexFileMessage( nCurrInputLine, QString("Invalid character found in record\n")));
            readHexError = true;
            // exit record
            eHexField = HEX_UNDEF;
            }
        }
        }
    }
    while(nByteRead);

    file.close();
    }
    else
    {
    // Error Msg
    m_sListErrWarn.append(THexFileMessage( nCurrInputLine, QString("File could not be opened\n")));
    readHexError = true;
    }

    return !readHexError;
}


void cIntelHexFileIOPrivate::GetMemoryBlock(const quint32& nBlockLen, quint32& nStartAddressModuloBlockLen, QByteArray& byteArray, quint32& nOffsetToModulo)
{
    quint32 nMaxAdress = nStartAddressModuloBlockLen + nBlockLen - 1;

    // Search regions affected
    QList<THexFileMemRegion> AffectedMemRegions;

    for (int i = 0; i != m_ListMemRegions.count(); i++)
    {
        THexFileMemRegion CurrEntry = m_ListMemRegions.at(i);
        if( (CurrEntry.nStartAddress >= nStartAddressModuloBlockLen  &&  CurrEntry.nStartAddress <=  nMaxAdress) || ( CurrEntry.GetMaxAddress() >= nStartAddressModuloBlockLen && CurrEntry.GetMaxAddress() <=nMaxAdress) ) // die region gehört zu der erwarteten speicherseite
            AffectedMemRegions.append(CurrEntry);
    }

    // In case there is no region matching, we have to find the region larger than start address
    if (AffectedMemRegions.count() == 0)
    {
        for (int i = 0; i != m_ListMemRegions.count(); i++)
        {
            THexFileMemRegion CurrEntry = m_ListMemRegions.at(i);
            if (CurrEntry.nStartAddress > nStartAddressModuloBlockLen)
            {
                AffectedMemRegions.append(CurrEntry);
                // Align adresses
                while (nMaxAdress < CurrEntry.nStartAddress)
                {
                    nMaxAdress += nBlockLen;
                    nStartAddressModuloBlockLen += nBlockLen;
                }
                break;
            }
        }

        // We finally found a first region: search for further affected (with new adresses)
        if (AffectedMemRegions.count())
        {
            //	posNext = AffectedMemRegions.begin(); // die region haben wir ja schon
            THexFileMemRegion entry = AffectedMemRegions[0];

            int next = m_ListMemRegions.indexOf(entry) + 1;
            while (next != m_ListMemRegions.count())
            {
                THexFileMemRegion CurrEntry =  m_ListMemRegions.at(next);
                if( (CurrEntry.nStartAddress >= nStartAddressModuloBlockLen  &&  CurrEntry.nStartAddress <=  nMaxAdress) || ( CurrEntry.GetMaxAddress() >= nStartAddressModuloBlockLen && CurrEntry.GetMaxAddress() <=nMaxAdress) ) // die region gehört zu der erwarteten speicherseite
                    AffectedMemRegions.append(CurrEntry);
                next++;
            }
        }
    }

    // Reset array (in case nothing was found an empty array shows caller that iteration is over)
    byteArray.resize(0);

    // The byte array has to be filled by the regions found
    if (AffectedMemRegions.count())
    {
        // At first we need to check for the memory range of the affected regions
        // Init conditions so that the first will change the ranges
        quint32 nRangeMinAddress = 0x7fffffff;
        quint32 nRangeMaxAddress = 0;
        int i;
        for ( i = 0; i < AffectedMemRegions.count(); i++)
        {
            THexFileMemRegion CurrEntry = AffectedMemRegions[i];
            if(CurrEntry.nStartAddress < nRangeMinAddress)
                nRangeMinAddress = CurrEntry.nStartAddress;
            if(CurrEntry.GetMaxAddress() > nRangeMaxAddress)
                nRangeMaxAddress = CurrEntry.GetMaxAddress();
        }

        // Limit the range to the new adresses
        if (nRangeMinAddress < nStartAddressModuloBlockLen)
            nRangeMinAddress = nStartAddressModuloBlockLen;
        if (nRangeMaxAddress > nMaxAdress)
            nRangeMaxAddress = nMaxAdress;

        // Setup our array
        byteArray.resize(nRangeMaxAddress-nRangeMinAddress+1);
        for( i = 0; i < byteArray.size(); i++)
            byteArray[i] = m_byteErasedByteValue;
        // Calc offset
        nOffsetToModulo = nRangeMinAddress - nStartAddressModuloBlockLen;

        // Fill the array from affected
        for ( i = 0; i < AffectedMemRegions.count(); i++)
        {
            THexFileMemRegion CurrEntry = AffectedMemRegions[i];
            ulong nFirstAddress;
            // | curr |          or   |    curr     |
            //		| desired |           | desired |
            if(CurrEntry.nStartAddress < nStartAddressModuloBlockLen)
                nFirstAddress = nStartAddressModuloBlockLen;
            //       | curr |		 or		    |  curr   |
            //     |  desired  |        | desired |
            else
                nFirstAddress = CurrEntry.nStartAddress;
            for(quint32 nCurrAddress = nFirstAddress; nCurrAddress <= CurrEntry.GetMaxAddress() && nCurrAddress <= nMaxAdress; nCurrAddress++)
            {
                byteArray[nCurrAddress - nStartAddressModuloBlockLen] = CurrEntry.ByteArrContent[nCurrAddress-CurrEntry.nStartAddress];
            }
        }
    }
}


void cIntelHexFileIOPrivate::setunwrittenByteValue(quint8 value)
{
    m_byteErasedByteValue = value;
}


QString cIntelHexFileIOPrivate::getErrorsWarnings()
{
    QString s;
    for (int i = 0; i < m_sListErrWarn.count(); i++ )
    {
        s += QString("%1: %2\n").arg(m_sListErrWarn.at(i).nLineNo).arg(m_sListErrWarn.at(i).sMsgText);
    }
    return s;
}


bool cIntelHexFileIOPrivate::isHexText(const char c)
{
    if ( ((c > 0x2F) && (c<0x3A)) || ((c > 0x40) && (c < 0x47)))
    return true;
    return false;
}


uchar cIntelHexFileIOPrivate::Hex2Bin(const char* c)
{
    uchar r;
    r=ASCII2Hex(*c) << 4;
    c++;
    r |= ASCII2Hex(*c);
    return r;
   //  return (ASCII2Hex(*c++) << 4) |  ASCII2Hex(*c);
}


uchar cIntelHexFileIOPrivate::ASCII2Hex(char c)
{
    uchar uc;
    uc = c - 0x30;
    if (uc < 0xA)
    return uc;
    uc -=7;
    return uc;
}

