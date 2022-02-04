#ifndef MESSAGETEXTS_H
#define MESSAGETEXTS_H

#include <QString>

class MessageTexts
{
public:
    enum Texts {
        ERR_SWITCH_ON,
        ERR_SWITCH_OFF,
        ERR_STATUS_POLL
    };
    static QString getText(Texts textId);
};

#endif // MESSAGETEXTS_H
