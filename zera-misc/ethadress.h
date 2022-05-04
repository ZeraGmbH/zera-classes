#ifndef ETHADRESS_H
#define ETHADRESS_H

#include "zeramisc_export.h"
#include <QDataStream>
#include <QString>

class cETHAdressPrivate;

class ZERAMISC_EXPORT cETHAdress
{
public:
    cETHAdress();
    cETHAdress(QString s);
    bool operator == (const cETHAdress& ethadr);
    bool operator != (const cETHAdress& ethadr);
    void operator = (const cETHAdress& ethadr);
    friend QDataStream& operator <<(QDataStream& ds, const cETHAdress& ethadr);
    friend QDataStream& operator >>(QDataStream& ds, const cETHAdress& ethadr);
    QString getMacAdress();
    bool setMacAdress(QString s);

private:
    cETHAdressPrivate* d_ptr;
};

QDataStream& operator <<(QDataStream& ds, const cETHAdress& ethadr);
QDataStream& operator >>(QDataStream& ds, const cETHAdress& ethadr);

#endif // ETHADRESS_H
