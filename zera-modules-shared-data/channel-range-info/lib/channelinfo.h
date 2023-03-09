#ifndef CHANNELINFO_H
#define CHANNELINFO_H

#include "rangeinfo.h"
#include <QObject>

class ChannelInfo : public QObject
{
    Q_OBJECT
public:
    ChannelInfo();
    QString getSysName() /* m0,m1...*/ const;
    QString getAlias() /* U1,I1...*/ const;
    int getDspChannel();
    //double getUserScale() const; // just an idea to remove module plastered crap...
    //void setUserScale(double userScaleFactor) const; // here - available for all?
    int getRangeCount() const;
    const RangeInfo* getRangeInfo(int rangeNo) const;
signals:
    void sigCurrentRangeChanged(int currentRangeNo);
    void sigRangesChanged(); /* added/removed */
    //void sigUserScaleChanged();
};

#endif // CHANNELINFO_H
