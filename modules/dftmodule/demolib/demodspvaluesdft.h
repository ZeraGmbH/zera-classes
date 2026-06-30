#ifndef DEMODSPVALUESDFT_H
#define DEMODSPVALUESDFT_H

#include <QStringList>
#include <QHash>
#include <complex>

class DemoDspValuesDft
{
public:
    DemoDspValuesDft(const QStringList &valueChannelList, int dftOrder);
    void setValue(QString valueChannelName, std::complex<double> value);
    void setAllValuesSymmetric(double voltage, double current, double angleUi, bool invertedSequence = false);
    void setAllValuesSymmetricDc(double voltage, double current);
    QVector<float> getDspValues();
private:
    static int nextSymmetricAngle(int angle);
    QStringList m_valueChannelList;
    int m_dftOrder;
    QHash<QString, std::complex<float>> m_values;
};

#endif // DEMODSPVALUESDFT_H
