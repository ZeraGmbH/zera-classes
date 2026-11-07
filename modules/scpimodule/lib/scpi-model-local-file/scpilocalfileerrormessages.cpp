#include "scpilocalfileerrormessages.h"

const QLatin1String ScpiLocalFileErrorMessages::InvalidFilePath("ERROR: invalid file path");
const QLatin1String ScpiLocalFileErrorMessages::FileNotFound("ERROR: file not found");
const QLatin1String ScpiLocalFileErrorMessages::NotARegularFile("ERROR: not a regular file");
const QLatin1String ScpiLocalFileErrorMessages::FileTooLarge("ERROR: file too large (max 1 MiB)");
const QLatin1String ScpiLocalFileErrorMessages::CannotOpenFile("ERROR: cannot open file");
const QLatin1String ScpiLocalFileErrorMessages::InvalidUtf8("ERROR: file is not valid UTF-8 text");
const QLatin1String ScpiLocalFileErrorMessages::ScpiParameterMissing("ERROR: missing parameter <filepath>");
const QLatin1String ScpiLocalFileErrorMessages::ScpiTooManyParameters("ERROR: only 1 parameter allowed");
