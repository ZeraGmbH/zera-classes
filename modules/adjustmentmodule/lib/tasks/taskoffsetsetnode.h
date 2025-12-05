#ifndef TASKSETOFFSETNODE_H
#define TASKSETOFFSETNODE_H

#include "tasktemplate.h"
#include <pcbinterface.h>

class TaskOffsetSetNode : public TaskTemplate
{
    Q_OBJECT
public:
    struct RangeWorkStorage
    {
        double m_offsetAdjCorrection;
        std::shared_ptr<double> m_rejection = std::make_shared<double>();
        std::shared_ptr<double> m_urValue = std::make_shared<double>();
    };
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  QString channelMName, QString rangeName,
                                  double actualValue, double targetValue, RangeWorkStorage &rngVals,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskOffsetSetNode(Zera::PcbInterfacePtr pcbInterface,
                      QString channelMName, QString rangeName,
                      double actualValue, double targetValue, RangeWorkStorage &rngVals);
    void start() override;
private slots:
    void onServerAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    Zera::PcbInterfacePtr m_pcbInterface;
    QString m_channelMName;
    QString m_rangeName;
    double m_actualValue;
    double m_targetValue;
    RangeWorkStorage &m_rngVals;
    quint32 m_msgnr = 0;
};

#endif // TASKSETOFFSETNODE_H
