#include "iosingletransferdata.h"

IOSingleTransferData::IOSingleTransferData()
{
}

bool IOSingleTransferData::operator ==(const IOSingleTransferData &other)
{
    return  m_responseTimeoutMs == other.m_responseTimeoutMs &&
            m_bytesSend == other.m_bytesSend &&
            m_bytesExpectedLead == other.m_bytesExpectedLead &&
            m_bytesExpectedTrail == other.m_bytesExpectedTrail;
}

