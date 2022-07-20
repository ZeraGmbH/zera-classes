#ifndef JUSTNODE_H
#define JUSTNODE_H

#include <QDataStream>
#include <QString>

// an adjustmentnode (datapoint) consists of the data point's value and the data point's argument
// it can serialize and deserialize itself to a qdatastream

class cJustNode { // stützspunkt kann sich serialisieren und besteht aus stützwert (correction) und argument
public:
    cJustNode(double corr, double arg);
    cJustNode(){};
    ~cJustNode(){};
    void Serialize(QDataStream&);
    void Deserialize(QDataStream&);
    QString Serialize(int digits);
    void Deserialize(const QString&);
    cJustNode& operator = (const cJustNode&);
    void setCorrection(double value);
    double getCorrection();
    void setArgument(double value);
    double getArgument();

private:
    double m_fCorrection;
    double m_fArgument;
};

#endif // JUSTNODE_H
