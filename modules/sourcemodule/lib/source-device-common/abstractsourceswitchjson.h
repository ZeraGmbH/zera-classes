#ifndef ABSTRACTSOURCESWITCHJSON_H
#define ABSTRACTSOURCESWITCHJSON_H

#include <jsonparamapi.h>
#include <QObject>

class AbstractSourceSwitchJson : public QObject
{
    Q_OBJECT
public:
    virtual int switchState(const JsonParamApi &paramState) = 0;
    virtual JsonParamApi getCurrLoadpoint() = 0;
signals:
    void sigSwitchFinished(bool ok, int switchId);
};

typedef std::shared_ptr<AbstractSourceSwitchJson> AbstractSourceSwitchJsonPtr;

#endif // ABSTRACTSOURCESWITCHJSON_H
