#include <math.h>
#include "complex.h"

double fabs(complex c)
{
    double re,im;
    re = c.re();
    im = c.im();
    return sqrt( (re*re) + (im*im) );
}

complex::complex()
    : fre(0.0), fim(0.0)
{}


complex::complex(double real, double imag)
    : fre(real), fim(imag)
{}


complex::complex(const complex& c)
    : fre(c.fre), fim (c.fim)
{}

double complex::fabs(const complex& c)
{
    return sqrt( (c.fre * c.fre) + (c.fim * c.fim));
}

complex complex::operator=(double f)
{
    fre = f;
    fim = 0.0;
    return *this;
}


complex complex::operator=(complex c)
{
    fre = c.fre;
    fim = c.fim;
    return *this;
}


complex complex::operator+(double f)
{
    complex rc(*this);
    rc.fre += f;
    return rc;
}


complex complex::operator+(complex c)
{
    complex rc(*this);
    rc.fre +=c.fre;
    rc.fim += c.fim;
    return rc;
}


complex complex::operator+=(double f)
{
    fre += f;
    return *this;
}


complex complex::operator+=(complex c)
{
    fre += c.fre;
    fim += c.fim;
    return *this;
}


complex complex::operator-(double f)
{
    complex rc(*this);
    rc.fre -=f;
    return rc;
}


complex complex::operator-(complex c)
{
    complex rc(*this);
    rc.fre -=c.fre;
    rc.fim -= c.fim;
    return rc;
}


complex complex::operator-=(double f)
{
    fre -= f;
    return *this;
}


complex complex::operator-=(complex c)
{
    fre -= c.fre;
    fim -= c.fim;
    return *this;
}


complex complex::operator*(double f)
{
    complex rc(*this);
    rc.fre *= f;
    rc.fim *= f;
    return rc;
}


complex complex::operator*(complex c)
{
    complex rc(*this);
    double re;
    re = rc.fre;
    rc.fre = re * c.fre - rc.fim * c.fim;
    rc.fim = re * c.fim + rc.fim * c.fre;
    return rc;
}


complex complex::operator*=(double f)
{
    fre *= f;
    fim *= f;
    return *this;
}


complex complex::operator*=(complex c)
{
    double re = fre;
    fre = re * c.fre - fim * c.fim;
    fim = re * c.fim + fim * c.fre;
    return *this;
}


complex complex::operator/(double f)
{
    complex rc(*this);
    rc.fre /= f;
    rc.fim /= f;
    return rc;
}


complex complex::operator/(complex c)
{
    complex rc(*this);
    double n = c.fre * c.fre + c.fim * c.fim;
    double re = rc.fre;
    rc.fre = (re * c.fre + rc.fim * c.fim)/n;
    rc.fim = (rc.fim * c.fre - re * c.fim)/n;
    return rc;
}


complex complex::operator/=(double f)
{
    fre /= f;
    fim /= f;
    return *this;
}


complex complex::operator/=(complex c)
{
    double n = c.fre * c.fre + c.fim * c.fim;
    double re = fre;
    fre = (re * c.fre + fim * c.fim)/n;
    fim = (fim * c.fre - re * c.fim)/n;
    return *this;
}
