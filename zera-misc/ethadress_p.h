#ifndef ETHADRESS_P_H
#define ETHADRESS_P_H

#include <QDataStream>
#include "ethadress.h"

class cETHAdressPrivate
{
public:
    cETHAdressPrivate(){}
    bool operator == (cETHAdressPrivate* p);
    bool operator != (cETHAdressPrivate* p);
    void operator = (cETHAdressPrivate* p);
    QString getMacAdress();
    bool setMacAdress(QString s);
    quint8 MacAdrByte[6];
};


#endif // ETHADRESS_P_H
