#ifndef ABSTRACTSOURCESWITCHJSON_H
#define ABSTRACTSOURCESWITCHJSON_H

#include <jsonparamapi.h>
#include <QObject>

class AbstractSourceSwitchJson : public QObject
{
    Q_OBJECT
public:
    virtual void switchState(JsonParamApi paramState) = 0;
    virtual void switchOff() = 0;
    virtual JsonParamApi getCurrLoadState() = 0;
    virtual JsonParamApi getRequestedLoadState() = 0;
signals:
    void sigSwitchFinished();
};

#endif // ABSTRACTSOURCESWITCHJSON_H
