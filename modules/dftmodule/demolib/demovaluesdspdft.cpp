#include "demovaluesdspdft.h"
#include "servicechannelnamehelper.h"

DemoValuesDspDft::DemoValuesDspDft(QStringList valueChannelList, int dftOrder) :
    m_valueChannelList(valueChannelList),
    m_dftOrder(dftOrder)
{
    for(const auto &valueName : valueChannelList)
        m_values.insert(valueName, std::complex<float>());
}

void DemoValuesDspDft::setValue(QString valueChannelName, std::complex<double> value)
{
    if(m_values.contains(valueChannelName))
        m_values[valueChannelName] = value;
}

void DemoValuesDspDft::setAllValuesSymmetric(double voltage, double current, double angleUi, bool invertedSequence)
{
    double multiplier = M_SQRT2;
    if(m_dftOrder == 0)
        // see comment in cDftModuleMeasProgram::dataReadDSP()
        multiplier = 2;

    QStringList voltageChannelNames = ServiceChannelNameHelper::getVoltageChannelNamesUsed(invertedSequence);
    int currAngle = 0;
    for(const auto &channel : qAsConst(voltageChannelNames)) {
        if(m_values.contains(channel)) {
            m_values[channel] = std::polar(voltage * multiplier, -double(currAngle) * M_PI / 180);
            currAngle = nextSymmetricAngle(currAngle);
        }
    }
    QStringList currentChannelNames = ServiceChannelNameHelper::getCurrentChannelNamesUsed(invertedSequence);
    currAngle = 0;
    for(const auto &channel : qAsConst(currentChannelNames)) {
        if(m_values.contains(channel)) {
            m_values[channel] = std::polar(current * multiplier, -(double(currAngle)+angleUi) * M_PI / 180);
            currAngle = nextSymmetricAngle(currAngle);
        }
    }
    // Note: Phase-phase values are (re-)calulated in cDftModuleMeasProgram::turnVectorsToRefChannel
}

QVector<float> DemoValuesDspDft::getDspValues()
{
    QVector<float> valueList;
    if(m_dftOrder != 0)
        for(const auto &valueChannelName : qAsConst(m_valueChannelList)) {
            valueList.append(m_values[valueChannelName].real());
            valueList.append(m_values[valueChannelName].imag());
        }
    else
        for(const auto &valueChannelName : qAsConst(m_valueChannelList)) {
            valueList.append(std::abs(m_values[valueChannelName]));
            valueList.append(0);
        }
    return valueList;
}

int DemoValuesDspDft::nextSymmetricAngle(int angle)
{
    angle += 120;
    if(angle >= 360)
        angle-=360;
    return angle;
}
