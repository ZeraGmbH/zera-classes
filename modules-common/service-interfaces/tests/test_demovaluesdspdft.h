#ifndef TEST_DEMOVALUESDSPDFT_H
#define TEST_DEMOVALUESDSPDFT_H

#include <complex>
#include <QObject>

class test_demovaluesdspdft : public QObject
{
    Q_OBJECT
private slots:
    void addVoltage();
    void addCurrent();
    void setSymmetric();
private:
    float gradToRad(float grad);
    std::complex<float> getDspValue(QString valueChannelName, QVector<float> dspValues);
    std::complex<float> getExpectedValue(float amplitude, float angle);
    static const QStringList mtRmsLayout;
};

#endif // TEST_DEMOVALUESDSPDFT_H
