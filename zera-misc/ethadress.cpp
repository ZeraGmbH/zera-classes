#include "ethadress.h"
#include "ethadress_p.h"


cETHAdress::cETHAdress()
    :d_ptr(new cETHAdressPrivate())
{
}


cETHAdress::cETHAdress(QString s)
    :d_ptr(new cETHAdressPrivate())
{
    d_ptr->setMacAdress(s);
}


bool cETHAdress::operator ==(const cETHAdress& ethadr)
{
    return d_ptr->operator == (ethadr.d_ptr);
}


bool cETHAdress::operator !=(const cETHAdress& ethadr)
{
    return d_ptr->operator != (ethadr.d_ptr);
}


void cETHAdress::operator = (const cETHAdress& ethadr)
{
    d_ptr->operator = (ethadr.d_ptr);
}


QDataStream& operator << (QDataStream& qds, const cETHAdress& eadr)
{
    for (int i = 0; i < 6; i++) qds << (quint8&) (eadr.d_ptr->MacAdrByte[i]);
    return qds;
}


QDataStream& operator >> (QDataStream& qds, const cETHAdress& eadr)
{
    for (int i = 0; i < 6; i++) qds >> (quint8&) (eadr.d_ptr->MacAdrByte[i]);
    return qds;
}


QString cETHAdress::getMacAdress()
{
    return d_ptr->getMacAdress();
}


bool cETHAdress::setMacAdress(QString s)
{
    return d_ptr->setMacAdress(s);
}
