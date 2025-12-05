#ifndef TASKOFFSETGETADJCORRECTION_H
#define TASKOFFSETGETADJCORRECTION_H

#include "tasktemplate.h"
#include <pcbinterface.h>

class TaskOffsetGetAdjCorrection : public TaskTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                   QString channelMName, QString rangeName, double ourActualValue,
                                   double &correctionValue,
                                   int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskOffsetGetAdjCorrection(Zera::PcbInterfacePtr pcbInterface,
                            QString channelMName, QString rangeName, double ourActualValue,
                            double &correctionValue);
    void start() override;
private slots:
    void onServerAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    Zera::PcbInterfacePtr m_pcbInterface;
    QString m_channelMName;
    QString m_rangeName;
    double m_ourActualValue;
    double &m_correctionValue;
    quint32 m_msgnr = 0;
};

#endif // TASKOFFSETGETADJCORRECTION_H
