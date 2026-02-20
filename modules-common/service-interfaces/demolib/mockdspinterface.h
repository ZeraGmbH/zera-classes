#ifndef MOCKDSPINTERFACE_H
#define MOCKDSPINTERFACE_H

#include "dspinterface.h"

class MockDspInterface : public Zera::cDSPInterface
{
    Q_OBJECT
public:
    MockDspInterface(int entityId = -1);
    void fireActValInterrupt(QVector<float> actualValues, int irqNo);
    quint32 dataAcquisition(cDspMeasData* memgroup) override;
    quint32 activateInterface() override;

signals:
    void sigDspMemoryWrite(QString name, QVector<float> values);
    void sigDspStarted();
protected:
    quint32 sendCmdResponse(QString answer);
private:
    QVector<float> m_actualValues;
};

typedef std::shared_ptr<MockDspInterface> MockDspInterfacePtr;

#endif // MOCKDSPINTERFACE_H
