#include "measmodeselector.h"

bool MeasModeSelector::addMode(std::shared_ptr<MeasMode> mode)
{
    if(mode->isValid()) {
        QString name = mode->getName();
        if(!m_modes.contains(name)) {
            m_modes[name] = mode;
            return true;
        }
    }
    return false;
}

void MeasModeSelector::tryChangeMode(QString modeName)
{
    if(m_modes.contains(modeName)) {
        m_currentMode = m_modes[modeName];
        emit sigTransactionOk();
    }
    else
        emit sigTransactionFailed();
}

std::shared_ptr<MeasMode> MeasModeSelector::getCurrMode() const
{
    if(m_currentMode)
        return m_currentMode;
    else
        return std::make_shared<MeasMode>();
}

bool MeasModeSelector::canChangeMask(QString mask) const
{
    return getCurrMode()->canChangeMask(mask);
}

void MeasModeSelector::tryChangeMask(QString mask)
{
    if(getCurrMode()->tryChangeMask(mask))
        emit sigTransactionOk();
    else
        emit sigTransactionFailed();
}

QString MeasModeSelector::getCurrentMask() const
{
    return getCurrMode()->getCurrentMask();
}

const QHash<QString, std::shared_ptr<MeasMode> > &MeasModeSelector::getModes() const
{
    return m_modes;
}
