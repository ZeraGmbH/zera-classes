#include "iotransferdatasingle.h"

IoTransferDataSingle::IoTransferDataSingle()
{
}

bool IoTransferDataSingle::operator ==(const IoTransferDataSingle &other)
{
    return  m_responseTimeoutMs == other.m_responseTimeoutMs &&
            m_bytesSend == other.m_bytesSend &&
            m_bytesExpectedLead == other.m_bytesExpectedLead &&
            m_bytesExpectedTrail == other.m_bytesExpectedTrail;
}

