#ifndef JSONFILENAMES_H
#define JSONFILENAMES_H

#include "supportedsources.h"
#include <QString>

class JsonFilenames
{
public:
    static QString getJsonStructurePath(SupportedSourceTypes type);
    static QString getJsonFileName(SupportedSourceTypes type);
};

#endif // JSONFILENAMES_H
