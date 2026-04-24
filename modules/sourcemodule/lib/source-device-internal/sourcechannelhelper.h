#ifndef SOURCECHANNELHELPER_H
#define SOURCECHANNELHELPER_H

#include <QStringList>
#include <jsonparamapi.h>
#include <jsonstructapi.h>

class SourceChannelHelper
{
public:
    static QStringList getChannelMNamesSwitchedOn(const JsonStructApi &sourceCapabilities,
                                                  const JsonParamApi &wantedLoadpoint);
    static QString getChannelMName(phaseType type, int phaseNoBase0);
    static QString getAlias(phaseType type, int phaseNoBase0);
};

#endif // SOURCECHANNELHELPER_H
