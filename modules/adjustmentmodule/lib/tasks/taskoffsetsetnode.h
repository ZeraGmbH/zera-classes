#ifndef TASKSETOFFSETNODE_H
#define TASKSETOFFSETNODE_H

#include "tasktemplate.h"
#include <pcbinterface.h>

class TaskOffsetSetNode : public TaskTemplate
{
    Q_OBJECT
public:
    struct RangeVals
    {
        double m_correction;
        double m_rejection;
        double m_rejectionValue;
    };
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                   QString channelMName, QString rangeName,
                                   double actualValue, double targetValue, RangeVals &rngVals,
                                   int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskOffsetSetNode(Zera::PcbInterfacePtr pcbInterface,
                      QString channelMName, QString rangeName,
                      double actualValue, double targetValue, RangeVals &rngVals);
    void start() override;
private slots:
    void onServerAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    Zera::PcbInterfacePtr m_pcbInterface;
    QString m_channelMName;
    QString m_rangeName;
    double m_actualValue;
    double m_targetValue;
    RangeVals &m_rngVals;
    quint32 m_msgnr;
};

#endif // TASKSETOFFSETNODE_H
