#ifndef DEMODSPVALUESOSCI_H
#define DEMODSPVALUESOSCI_H

#include <QVector>
#include <QStringList>

class DemoDspValuesOsci
{
public:
    explicit DemoDspValuesOsci(const QStringList &valueChannelList);
    explicit DemoDspValuesOsci(int entityId,
                               const QStringList &valueChannelList,
                               int sampleCount,
                               std::function<double(int)> valueGenerator);

    void setAllValuesSymmetricAc(float voltage, float current);
    void setAllValuesSymmetricDc(float voltage, float current);

    QVector<float> getDspValues() const;

private:
    static QVector<float> generateSineCurve(int sampleCount, int amplitude, float phase);
    static QVector<float> generateDcCurve(int sampleCount, int amplitude);

    int m_entityId = 0;
    QStringList m_valueChannelList;
    int m_sampleCount = 128;
    std::function<double(int)> m_valueGenerator;

    QVector<float> m_powerValues;
};

#endif // DEMODSPVALUESOSCI_H
