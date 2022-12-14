#ifndef TASKOFFSETGETCORRECTION_H
#define TASKOFFSETGETCORRECTION_H

#include "taskcomposit.h"
#include "pcbinterface.h"

class TaskOffsetGetCorrection : public TaskComposite
{
    Q_OBJECT
public:
    static TaskCompositePtr create(Zera::Server::PcbInterfacePtr pcbInterface,
                                   QString channelSysName, QString rangeName, double ourActualValue,
                                   double &correctionValue,
                                   int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskOffsetGetCorrection(Zera::Server::PcbInterfacePtr pcbInterface,
                            QString channelSysName, QString rangeName, double ourActualValue,
                            double &correctionValue);
    void start() override;
private slots:
    void onRmAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    Zera::Server::PcbInterfacePtr m_pcbInterface;
    QString m_channelSysName;
    QString m_rangeName;
    double m_ourActualValue;
    double &m_correctionValue;
    quint32 m_msgnr;
};

#endif // TASKOFFSETGETCORRECTION_H
