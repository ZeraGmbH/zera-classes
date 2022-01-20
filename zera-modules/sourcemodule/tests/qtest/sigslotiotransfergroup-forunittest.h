#ifndef SIGSLOTIOTRANSFERGROUP_H
#define SIGSLOTIOTRANSFERGROUP_H

#include "io-device/iotransferdatagroup.h"
#include <QObject>

class SigSlotIoTransferGroup : public QObject
{
    Q_OBJECT
public:
    SigSlotIoTransferGroup(bool queued);

    IoTransferDataGroup m_groupSend;
    IoTransferDataGroup m_groupReceived;
    int m_receiveCount = 0;

signals:
    void sigTransferGroup(IoTransferDataGroup transferGroup);
public slots:
    void onTransferGroup(IoTransferDataGroup transferGroup);
};

#endif // SIGSLOTIOTRANSFERGROUP_H
