#ifndef SECINPUTINFO_H
#define SECINPUTINFO_H

#include <QString>

struct SecInputInfo
{
    QString name; // the input's system name
    QString alias; // the input's alias
    QString resource; // the resource from which this input is served
};

#endif // SECINPUTINFO_H
