#include "zera_mcontroller_errorflags.h"
#include "protocol_zera_bootloader.h"
#include "protocol_zera_hard.h"

QHash<quint32, QString> ZeraMcontrollerErrorFlags::m_errorFlagsText;
QHash<quint32, QString> ZeraMcontrollerErrorFlags::m_errorFlagsBootText;

QString ZeraMcontrollerErrorFlags::getErrorMaskText(quint32 errorFlags, bool bootCmd)
{
    fillTextHashes();
    quint16 errMaskMaster = errorFlags >> 16;
    quint16 errMaskMController = errorFlags & 0xFFFF;
    QString strError;
    QString strFlags;
    for(int iFlag = 0; iFlag<32; ++iFlag) {
        if(errorFlags & (1<<iFlag)) {
            if(!strFlags.isEmpty()) {
                strFlags += QStringLiteral(" | ");
            }
            strFlags += bootCmd ? m_errorFlagsBootText[1<<iFlag] : m_errorFlagsText[1<<iFlag];
        }
    }
    strError.sprintf("host-mask 0x%04X: / µC-mask 0x%04X / %s",
                     errMaskMaster,
                     errMaskMController,
                     qPrintable(strFlags));
    return strError;
}

void ZeraMcontrollerErrorFlags::fillTextHashes()
{
    if(m_errorFlagsText.isEmpty()) {
        m_errorFlagsText.insert(HW_ERR_FLAG_CRC,              QStringLiteral("HW_ERR_FLAG_CRC"));
        m_errorFlagsText.insert(HW_ERR_FLAG_LENGTH,           QStringLiteral("HW_ERR_FLAG_LENGTH"));
        m_errorFlagsText.insert(HW_ERR_CMD_FOR_DEV_NOT_AVAIL, QStringLiteral("HW_ERR_CMD_FOR_DEV_NOT_AVAIL"));
        m_errorFlagsText.insert(HW_ERR_FLAG_EXECUTE,          QStringLiteral("HW_ERR_FLAG_EXECUTE"));
        m_errorFlagsText.insert(HW_ERR_LAYER2,                QStringLiteral("HW_ERR_LAYER2"));
        m_errorFlagsText.insert(HW_ERR_PARAM_INVALID,         QStringLiteral("HW_ERR_PARAM_INVALID"));
        m_errorFlagsText.insert(HW_ERR_ASCII_HEX,             QStringLiteral("HW_ERR_ASCII_HEX"));
        appendMasterErrorFlags(m_errorFlagsText);
    }
    if(m_errorFlagsBootText.isEmpty()) {
        m_errorFlagsBootText.insert(BL_ERR_FLAG_BUFFER_OVERFLOW, QStringLiteral("BL_ERR_FLAG_BUFFER_OVERFLOW"));
        m_errorFlagsBootText.insert(BL_ERR_FLAG_HEX_INVALID,     QStringLiteral("BL_ERR_FLAG_HEX_INVALID"));
        m_errorFlagsBootText.insert(BL_ERR_FLAG_LENGTH,          QStringLiteral("BL_ERR_FLAG_LENGTH"));
        m_errorFlagsBootText.insert(BL_ERR_FLAG_CRC,             QStringLiteral("BL_ERR_FLAG_CRC"));
        m_errorFlagsBootText.insert(BL_ERR_FLAG_CMD_INVALID,     QStringLiteral("BL_ERR_FLAG_CMD_INVALID"));
        m_errorFlagsBootText.insert(BL_ERR_FLAG_ADR_RANGE,       QStringLiteral("BL_ERR_FLAG_ADR_RANGE"));
        m_errorFlagsBootText.insert(BL_ERR_FLAG_EXECUTE,         QStringLiteral("BL_ERR_FLAG_EXECUTE"));
        appendMasterErrorFlags(m_errorFlagsBootText);
    }

}

void ZeraMcontrollerErrorFlags::appendMasterErrorFlags(QHash<quint32, QString> &errorFlagsText)
{
    errorFlagsText.insert(MASTER_ERR_FLAG_I2C_TRANSFER,      QStringLiteral("MASTER_ERR_FLAG_I2C_TRANSFER"));
    errorFlagsText.insert(MASTER_ERR_FLAG_CRC,               QStringLiteral("MASTER_ERR_FLAG_CRC"));
    errorFlagsText.insert(MASTER_ERR_FLAG_LENGTH,            QStringLiteral("MASTER_ERR_FLAG_LENGTH"));
    errorFlagsText.insert(MASTER_ERR_FLAG_FLASH_WRITE,       QStringLiteral("MASTER_ERR_FLAG_FLASH_WRITE"));
    errorFlagsText.insert(MASTER_ERR_FLAG_VERIFY_BLOCK,      QStringLiteral("MASTER_ERR_FLAG_VERIFY_BLOCK"));
    errorFlagsText.insert(MASTER_ERR_FLAG_EERPOM_WRITE,      QStringLiteral("MASTER_ERR_FLAG_EERPOM_WRITE"));
    errorFlagsText.insert(MASTER_ERR_FLAG_FLASH_VERIFY,      QStringLiteral("MASTER_ERR_FLAG_FLASH_VERIFY"));
    errorFlagsText.insert(MASTER_ERR_FLAG_EERPOM_VERIFY,     QStringLiteral("MASTER_ERR_FLAG_EERPOM_VERIFY"));
    errorFlagsText.insert(MASTER_ERR_FLAG_CODER_IS_AN_IDIOT, QStringLiteral("MASTER_ERR_FLAG_CODER_IS_AN_IDIOT"));
}
