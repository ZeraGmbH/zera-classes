#include "ethadress_p.h"


bool cETHAdressPrivate::operator == (cETHAdressPrivate* p)
{
    bool eq = true;
    for (int i = 0; i < 6; i++)
        eq &= (p->MacAdrByte[i] == this->MacAdrByte[i]);
    return eq;
}


bool cETHAdressPrivate::operator != (cETHAdressPrivate *p)
{
    bool ne = false;
    for (int i = 0; i < 6; i++)
        ne |= (p->MacAdrByte[i] != this->MacAdrByte[i]);
    return ne;
}


void cETHAdressPrivate::operator = (cETHAdressPrivate *p)
{
    for (int i = 0; i < 6; i++)
        this->MacAdrByte[i] = p->MacAdrByte[i];

}


QString cETHAdressPrivate::getMacAdress()
{
    int i;
    QString s;

    for (i = 0; i < 5; i++)
        s += QString("%1,").arg(MacAdrByte[i],0,16);
    s += QString("%1").arg(MacAdrByte[i],0,16);

    return s;
}


bool cETHAdressPrivate::setMacAdress(QString s)
{
    int i;
    QString ls;
    ushort us;
    cETHAdressPrivate leth;
    for (i = 0; i < 6 ; i++)
    {
        ls = s.section(',',i,i);
        if (ls.isEmpty() ) // wenn 1 byte fehlt -> fehler
            break;
        us = ls.toUShort(0,16);
        if ( us >255 ) // werte über ff -> fehler
            break;
        leth.MacAdrByte[i] = us;
    }

    if (i < 6)
    return false;

    for (i = 0; i < 6 ; i++)
    this->MacAdrByte[i] = leth.MacAdrByte[i];

    return true;
}


