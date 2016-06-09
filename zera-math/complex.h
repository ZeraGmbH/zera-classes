#ifndef COMPLEX_H
#define COMPLEX_H

class complex;

double fabs(complex);

class complex
{
public:
    complex();
    complex(const double real,const double imag = 0.0);
    complex(const complex&);

    double re() {return fre;}
    double im() {return fim;}
    double fabs(const complex&);

    complex operator=(double);
    complex operator=(complex);

    complex operator+(double);
    complex operator+(complex);
    complex operator+=(double);
    complex operator+=(complex);

    complex operator-(double);
    complex operator-(complex);
    complex operator-=(double);
    complex operator-=(complex);

    complex operator*(double);
    complex operator*(complex);
    complex operator*=(double);
    complex operator*=(complex);

    complex operator/(double);
    complex operator/(complex);
    complex operator/=(double);
    complex operator/=(complex);

private:
    double fre; // realteil
    double fim; // imaginärteil
};

#endif // COMPLEX_H

