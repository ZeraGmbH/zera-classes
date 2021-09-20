#ifndef CSOURCEDEVICEMANAGER_H
#define CSOURCEDEVICEMANAGER_H

#include <QObject>
#include <QString>

class cIOInterface;

namespace SOURCEMODULE
{

class cSourceDevice;
class cSourceConnectTransaction;

class cSourceDeviceManager : public QObject
{
    Q_OBJECT
public:
    cSourceDeviceManager(int countSlots, QObject *parent = 0);

    void startSourceIdentification(cIOInterface *interface);

    int activeSlotCount();
    cSourceDevice* getSourceDevice(int slotNo);

signals:
    void sigSlotAdded(int slotNo);
    void sigSlotRemoved(int slotNo);

private slots:
    void onIdentificationTransactionFinished(cSourceConnectTransaction* transaction);
    void onRemoveSource(cSourceDevice *sourceDevice);

private:
    void addSource(cSourceDevice* sourceDevice);

    QVector<cSourceDevice*> m_sourceDeviceSlots;
    int m_activeSlotCount;
};

}

#endif // CSOURCEDEVICEMANAGER_H
