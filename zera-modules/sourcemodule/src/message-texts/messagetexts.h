#ifndef MESSAGETEXTS_H
#define MESSAGETEXTS_H

#include <QString>

class MessageTexts
{
public:
    enum Texts {
        ERR_SWITCH_ON,
        ERR_SWITCH_OFF
    };
    static QString getText(Texts textId);
};

#endif // MESSAGETEXTS_H
