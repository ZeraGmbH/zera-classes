#ifndef TASKOFFSETGETCORRECTION_H
#define TASKOFFSETGETCORRECTION_H

#include "tasktemplate.h"
#include <pcbinterface.h>

class TaskOffsetGetCorrection : public TaskTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                   QString channelMName, QString rangeName, double ourActualValue,
                                   double &correctionValue,
                                   int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskOffsetGetCorrection(Zera::PcbInterfacePtr pcbInterface,
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
    quint32 m_msgnr;
};

#endif // TASKOFFSETGETCORRECTION_H
