#ifndef TASKSETOFFSETNODE_H
#define TASKSETOFFSETNODE_H

#include "taskcomposit.h"
#include "pcbinterface.h"

class TaskOffsetSetNode : public TaskComposite
{
    Q_OBJECT
public:
    struct RangeVals
    {
        double m_correction;
        double m_rejection;
        double m_rejectionValue;
    };
    static TaskCompositePtr create(Zera::Server::PcbInterfacePtr pcbInterface,
                                   QString channelSysName, QString rangeName,
                                   double actualValue, double targetValue, RangeVals &rngVals,
                                   int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskOffsetSetNode(Zera::Server::PcbInterfacePtr pcbInterface,
                      QString channelSysName, QString rangeName,
                      double actualValue, double targetValue, RangeVals &rngVals);
    void start() override;
private slots:
    void onServerAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    Zera::Server::PcbInterfacePtr m_pcbInterface;
    QString m_channelSysName;
    QString m_rangeName;
    double m_actualValue;
    double m_targetValue;
    RangeVals &m_rngVals;
    quint32 m_msgnr;
};

#endif // TASKSETOFFSETNODE_H
