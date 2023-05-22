#ifndef ZeraMcontrollerErrorFlags_H
#define ZeraMcontrollerErrorFlags_H

#include <QString>
#include <QHash>

/** @brief Master-side error flags
 */
constexpr quint32 MASTER_ERR_FLAG_I2C_TRANSFER = 1<<16;
constexpr quint32 MASTER_ERR_FLAG_CRC = 1<<17;
constexpr quint32 MASTER_ERR_FLAG_LENGTH = 1<<18;
constexpr quint32 MASTER_ERR_FLAG_VERIFY_BLOCK = 1<<19;
constexpr quint32 MASTER_ERR_FLAG_FLASH_WRITE = 1<<20;
constexpr quint32 MASTER_ERR_FLAG_EERPOM_WRITE = 1<<21;
constexpr quint32 MASTER_ERR_FLAG_FLASH_VERIFY = 1<<22;
constexpr quint32 MASTER_ERR_FLAG_EERPOM_VERIFY = 1<<23;

constexpr quint32 MASTER_ERR_FLAG_CODER_IS_AN_IDIOT = 1<<31;

class ZeraMcontrollerErrorFlags
{
public:
    static QString getErrorMaskText(quint32 errorFlags, bool bootCmd);

private:
    static void fillTextHashes();
    static void appendMasterErrorFlags(QHash<quint32, QString>& errorFlagsText);

    static QHash<quint32, QString> m_errorFlagsText;
    static QHash<quint32, QString> m_errorFlagsBootText;
};

#endif // ZeraMcontrollerErrorFlags_H
