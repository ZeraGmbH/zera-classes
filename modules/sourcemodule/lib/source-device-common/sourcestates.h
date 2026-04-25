#ifndef SOURCESTATES_H
#define SOURCESTATES_H

enum class SourceStates
{
    UNDEFINED,
    IDLE,
    SWITCH_BUSY,
    ERROR_SWITCH,
    ERROR_POLL
};

#endif // SOURCESTATES_H
