#include "measmodeselector.h"

bool MeasModeSelector::addMode(std::shared_ptr<MeasMode> mode)
{
    if(mode->getInfo().isValid()) {
        QString name = mode->getInfo().getName();
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
        emit sigModeChanged();
    }
    else
        emit sigModeChangeFailed();
}

std::shared_ptr<MeasMode> MeasModeSelector::getCurrMode()
{
    return m_currentMode;
}
