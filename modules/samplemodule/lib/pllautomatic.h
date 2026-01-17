#ifndef PLLAUTOMATIC_H
#define PLLAUTOMATIC_H

#include "rangemodulevalueobserver.h"
#include <vs_abstracteventsystem.h>

class PllAutomatic : public QObject
{
    Q_OBJECT
public:
    static constexpr float RelativeAmplitudeMinForPllChannelSelection = 0.1;

    explicit PllAutomatic(VeinStorage::AbstractEventSystem* veinStorage,
                          const QStringList &channelMNameList);
    void activate(bool on);
signals:
    void sigSelectPllChannel(QString channelMName);

private slots:
    void onNewValues();
private:
    void tryEmitPllChannel();
    void tryEmitDefaultPllChannel();

    bool m_active = false;
    QStringList m_channelMNameList;
    RangeModuleValueObserver m_valueObserver;
};

#endif // PLLAUTOMATIC_H
