#include "sigslotiotransfergroup-forunittest.h"

SigSlotIoTransferGroup::SigSlotIoTransferGroup(bool queued)
{
    IoTransferDataGroup workTransferGroup(IoTransferDataGroup::GROUP_TYPE_SWITCH_OFF, IoTransferDataGroup::BEHAVE_STOP_ON_ERROR);
    tIoTransferList transList;
    transList.append(tIoTransferDataSingleShPtr(new IoTransferDataSingle("1a","1b", "1c", 1, false)));
    transList.append(tIoTransferDataSingleShPtr(new IoTransferDataSingle("2a","2b", "2c", 2, true)));
    workTransferGroup.appendTransferList(transList);
    m_groupSend = workTransferGroup;

    connect(this, &SigSlotIoTransferGroup::sigTransferGroup,
            this, &SigSlotIoTransferGroup::onTransferGroup,
            queued ? Qt::QueuedConnection : Qt::DirectConnection);
    emit sigTransferGroup(m_groupSend);
}

void SigSlotIoTransferGroup::onTransferGroup(IoTransferDataGroup transferGroup)
{
    m_groupReceived = transferGroup;
    m_receiveCount++;
}
