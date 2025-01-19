#ifndef TEST_DFT_DEMOVALUES_H
#define TEST_DFT_DEMOVALUES_H

#include <complex>
#include <QObject>

class test_dft_demovalues : public QObject
{
    Q_OBJECT
private slots:
    void addVoltage();
    void addCurrent();
    void setSymmetric();
private:
    double gradToRad(double grad);
    std::complex<float> getDspValue(QString valueChannelName, QVector<float> dspValues);
    std::complex<float> getExpectedValue(double amplitude, double angle);
    static const QStringList mtRmsLayout;
};

#endif // TEST_DEMOVALUESDSPDFT_H
