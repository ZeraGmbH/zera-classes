#ifndef DEMOVALUESDSPDFT_H
#define DEMOVALUESDSPDFT_H

#include <QStringList>
#include <QHash>
#include <complex>

class DemoValuesDspDft
{
public:
    DemoValuesDspDft(QStringList valueChannelList, int dftOrder);
    void setValue(QString valueChannelName, std::complex<float> value);
    void setAllValuesSymmetric(float voltage, float current, float angleUi, bool invertedSequence = false);
    QVector<float> getDspValues();
private:
    int nextSymmetricAngle(int angle);
    QStringList m_valueChannelList;
    int m_dftOrder;
    QHash<QString, std::complex<float>> m_values;
};

#endif // DEMOVALUESDSPDFT_H
