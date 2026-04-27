#ifndef ABSTRACTSOURCESWITCHJSON_H
#define ABSTRACTSOURCESWITCHJSON_H

#include <jsonparamapi.h>
#include <QObject>

class AbstractSourceSwitchJson : public QObject
{
    Q_OBJECT
public:
    virtual void switchState(const JsonParamApi &paramState) = 0;
    virtual JsonParamApi getCurrLoadpoint() = 0;
    virtual JsonParamApi getRequestedLoadState() = 0;
signals:
    void sigSwitchFinished(bool ok);
};

#endif // ABSTRACTSOURCESWITCHJSON_H
