#ifndef MEASCHANNEL_H
#define MEASCHANNEL_H

#include <QObject>

class MeasChannel : public QObject
{
    Q_OBJECT
public:
    explicit MeasChannel(QObject *parent = nullptr);

signals:

};

#endif // MEASCHANNEL_H
