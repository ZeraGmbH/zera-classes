#include "messagetexts.h"

QString MessageTexts::getText(MessageTexts::Texts textId)
{
    QString text;
    switch(textId) {
    case ERR_SWITCH_ON:
        text = QStringLiteral("Switch on failed");
        break;
    case ERR_SWITCH_OFF:
        text = QStringLiteral("Switch off failed");
        break;
    }
    return text;
}
