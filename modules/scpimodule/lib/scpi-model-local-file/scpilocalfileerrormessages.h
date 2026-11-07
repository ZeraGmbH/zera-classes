#ifndef SCPILOCALFILEERRORMESSAGES_H
#define SCPILOCALFILEERRORMESSAGES_H

#include <QLatin1String>

class ScpiLocalFileErrorMessages
{
public:
    static const QLatin1String InvalidFilePath;
    static const QLatin1String FileNotFound;
    static const QLatin1String NotARegularFile;
    static const QLatin1String FileTooLarge;
    static const QLatin1String CannotOpenFile;
    static const QLatin1String InvalidUtf8;
    static const QLatin1String ScpiParameterMissing;
    static const QLatin1String ScpiTooManyParameters;
};

#endif // SCPILOCALFILEERRORMESSAGES_H
