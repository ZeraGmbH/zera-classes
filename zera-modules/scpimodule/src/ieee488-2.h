#ifndef IEEE4882_H
#define IEEE4882_H

#include <QObject>

namespace SCPIMODULE
{

const quint16 STBOPER = 0x80;
const quint16 STBRQSMSS = 0x40;
const quint16 STBESB = 0x20;
const quint16 STBMAV = 0x10;
const quint16 STBQUES = 0x8;
const quint16 STBBIT2 = 0x4;
const quint16 STBBIT1 = 0x2;
const quint16 STBBIT0 = 0x1;


enum opcStates { OCIS, // operation idle state
                 OCAS, // operation active state
                 OQIS, // operation query idle state
                 OQAS }; // operation query active state


class cIEEE4482: public QObject
{
    Q_OBJECT

public:
    cIEEE4482();

private:
    QString RegOutput(quint8 reg);
    quint16 m_nQueueLen; // max. anzahl einträge in obiger queue

    quint8 m_nSTB; // status byte !!!!!! setting/resetting these 4 registers must be done using dedicated functions
    quint8 m_nSRE; // service reguest enable
    quint8 m_nESR; // standard event status register
    quint8 m_nESE; // standard event status enable
    opcStates m_nOPCState;
    opcStates m_nOPCQState;

    bool m_bnoOperationPendingFlag; // use dedicated function for setting/resetting
};

}
#endif // IEEE4882_H
