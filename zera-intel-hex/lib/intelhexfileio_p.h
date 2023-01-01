#ifndef INTELHEXFILEIO_P_H
#define INTELHEXFILEIO_P_H

#include <QString>
#include <QList>

struct THexFileMemRegion
{
    // data
    quint32 nStartAddress;
    QByteArray ByteArrContent;
    // methods
    THexFileMemRegion();
    THexFileMemRegion(quint32 StartAddress, const QByteArray& byteArray);
    THexFileMemRegion& operator = (const THexFileMemRegion& obj);
    bool operator == (const THexFileMemRegion& obj) const;
    quint32 GetMaxAddress();
};


struct THexFileMessage
{
    // data
    quint32 nLineNo;
    QString sMsgText;				// either string
    // methods
    THexFileMessage(){}
    THexFileMessage(quint32 _nLineNo, const QString& _sMsgText);
};


class cIntelHexFileIOPrivate
{
public:
    cIntelHexFileIOPrivate();
    /**
      @b Read the entire file and parse for memory regions.
      @param fileName is the files full path name.
      */
    bool ReadHexFile(const QString& fileName);
    /**
      @b Set byte value which is used to set unwritten regions.
      */
    void setunwrittenByteValue(quint8 value);
    QString getErrorsWarnings();

    /**
      Memory Read iterated: In case more than one block meet address range: Stuff empty regions between
      with unwritten byte value. End is signalled by an empty array. If no block is found for the adress,
      he address is incremented to the next block found in steps of dwBlockLen to ensure to write one
      flash block only once. By adding dwStartAddressModuloBlockLen+dwOffsetToModulo the effective
      start address of the memory block in byteArray is calculated.
      */
    bool GetMemoryBlock(const quint32& nBlockLen, quint32& nStartAddressModuloBlockLen, QByteArray& byteArray, quint32& nOffsetToModulo);
    quint32 getByteCountStuffed();
    /**
     * @brief isEmpty
     * @return true if no memory reagions were read
     */
    bool isEmpty();
    bool contains(const QByteArray &byteArray);


private:
    bool isHexText(const char c);
    quint8 Hex2Bin(const char* c);
    quint8 ASCII2Hex(char c);
    QList<THexFileMemRegion> m_ListMemRegions;
    QList<THexFileMessage> m_sListErrWarn;
    QString m_strFileName;
    quint8 m_byteErasedByteValue;
    quint16 m_countBytesStuffed;
};


#endif // INTELHEXFILEIO_P_H
