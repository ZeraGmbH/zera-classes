#include "intelhexfileio.h"
#include "intelhexfileio_p.h"


cIntelHexFileIO::cIntelHexFileIO()
    :d_ptr( new cIntelHexFileIOPrivate() )
{
}


cIntelHexFileIO::~cIntelHexFileIO()
{
    delete d_ptr;
}


bool cIntelHexFileIO::ReadHexFile(const QString& fileName)
{
    return d_ptr->ReadHexFile(fileName);
}


void cIntelHexFileIO::GetMemoryBlock(const quint32& nBlockLen, quint32& nStartAddressModuloBlockLen, QByteArray& byteArray, quint32& nOffsetToModulo)
{
    d_ptr->GetMemoryBlock( nBlockLen, nStartAddressModuloBlockLen, byteArray, nOffsetToModulo);
}


void cIntelHexFileIO::setunwrittenByteValue(quint8 value)
{
    d_ptr->setunwrittenByteValue(value);
}


QString cIntelHexFileIO::getErrorsWarnings()
{
    return d_ptr->getErrorsWarnings();
}
