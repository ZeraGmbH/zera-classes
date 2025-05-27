#ifndef SOURCESWITCHJSONINTERNAL_H
#define SOURCESWITCHJSONINTERNAL_H

#include <abstractsourceswitchjson.h>

class SourceSwitchJsonInternal : public AbstractSourceSwitchJson
{
    Q_OBJECT
public:
    SourceSwitchJsonInternal();
    void switchState(JsonParamApi paramState) override;
    void switchOff() override;
    JsonParamApi getCurrLoadState() override;
    JsonParamApi getRequestedLoadState() override;
};

#endif // SOURCESWITCHJSONINTERNAL_H
