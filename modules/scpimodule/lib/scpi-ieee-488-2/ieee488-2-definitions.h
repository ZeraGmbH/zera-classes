#ifndef IEEE488_3_DEFINITIONS_H
#define IEEE488_3_DEFINITIONS_H

namespace SCPIMODULE
{

enum scpicommoncommands
{
    operationComplete,
    eventstatusenable,
    servicerequestenable,
    clearstatus,
    reset,
    identification,
    eventstatusregister,
    statusbyte,
    selftest,
    read1error,
    readerrorcount,
    readallerrors
};

enum scpiErrorInd
{
    NoError,
    CommandError,
    InvalidSeparator,
    ParameterNotAllowed,
    MissingParameter,
    UndefinedHeader,
    NumericDataError,

    ExecutionError,
    CommandProtected,
    HardwareError,
    HardwareMissing,
    FileNameError,

    DeviceSpecificError,
    systemError,
    QueueOverflow,
    QueryError,
    PowerOn,
    scpiLastError
};

struct scpiErrorType
{
    short ErrNum; // fehler nummer (negativ)
    char* ErrTxt; // fehler text
};

// Page 160 famous John M. Pieper book ISBN-13: 978-3939837022
enum STBBitPositions    // Status byte
{
    STBbit0,            // Device dependent - unused here
    STBbit1,            // Device dependent - unused here
    STBeeQueueNotEmpty, // Error queue not empty - cSCPIClient creates a cSCPIStatus for this bit position
    STBques,            // Questionable (=non fatal errors / warnings) - cSCPIClient creates a cSCPIStatus for this bit position
    STBmav,             // Message available - unused here (just cleared in ClearEventError())
    STBesb,             // Event summary
    STBrqs,             // Requested service (or MSS Master summary status)
    STBoper             // Operation status
};

// Page 168
enum SESRBitValues
{
    SESROperationComplete = 1,
    SESRRequestControl = 2,
    SESRQueryError = 4,
    SESRDevDepError = 8,
    SESRExecError = 16,
    SESRCommandError = 32,
    SESRUserRequest = 64,
    SESRPowerOn = 128
};



};
#endif // IEEE488_3_DEFINITIONS_H
