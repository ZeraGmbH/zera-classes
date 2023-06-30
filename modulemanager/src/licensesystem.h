#ifndef LICENSESYSTEM_H
#define LICENSESYSTEM_H

#include "modman_util.h"

#include <ve_eventsystem.h>

#include <QMap>
#include <QHash>
#include <QUrl>

class LicenseSystem : public VeinEvent::EventSystem
{
    Q_OBJECT
public:
    LicenseSystem(const QSet<QUrl> &t_licenseURLs, QObject *t_parent = nullptr);

    bool isSystemLicensed(const QString &t_uniqueModuleName);
    /**
   * @brief Returns the data of the license json objects for the given system
   * @param t_systemName
   * @return empty QVariantMap() if no system exists with t_systemName
   */
    QVariantMap systemLicenseConfiguration(const QString &t_systemName) const;

    void setDeviceSerial(const QString &t_serialNumber);
    /**
   * @brief serialNumberIsInitialized
   * @return true if the StatusModule1.PAR_SerialNr was retrieved
   */
    bool serialNumberIsInitialized() const;

signals:
    /**
   * @brief Called once the StatusModule1.PAR_SerialNr is retrieved
   */
    void sigSerialNumberInitialized();

private:
    /**
   * @brief loads the license data from ":/license_cert.pem"
   * @return
   */
    QByteArray loadCertData() const;
    /**
   * @brief reads a license file from the path
   * @param t_filePath
   * @return
   */
    QByteArray loadLicenseFile(const QString &t_filePath) const;
    /**
   * @brief reads all license files in a directory via loadLicenseFile() into a hash
   * @param t_path
   * @return key: filename, value: license data
   */
    QHash<QString, QByteArray> getLicenseFilesFromPath(const QString &t_path) const;

    /**
   * @brief Validates the expiry date against QDateTime::currentDateTime()
   * @param t_dateString
   * @return success indicator
   */
    bool isValidLicenseExpiryDate(const QString t_dateString) const;
    /**
   * @brief Validates the device serial against the StatusModule1.PAR_SerialNr
   * @param t_deviceSerial
   * @return success indicator
   */
    bool isValidLicenseDeviceSerial(const QString t_deviceSerial) const;

    ///@todo allows multiple paths to load licenses from, but lacks implementations for them (e.g. http://$LICENSE_SERVER:8080/licenses/$SERIALNO)
    const QSet<QUrl> m_licenseURLs;

    //modules currently don't support configurable licensing
    QList<QString> m_licensedSystems;

    //use QVariantMap for support of QML type conversion
    ///@note verified only means that the file and signature is valid, not that the license has correct expiry date and serial number
    QHash<QString, QVariantMap> m_verifiedLicenseDataTable;

    //signer x509 certificate
    QByteArray m_certData;

    QString m_deviceSerial;

    bool m_universalLicenseFound=false;
    bool m_serialNumberInitialized=false;

    static constexpr QLatin1String s_systemNameDescriptor = modman_util::to_latin1("uniqueSystemName");
    static constexpr QLatin1String s_expiresDescriptor = modman_util::to_latin1("expires");
    static constexpr QLatin1String s_expiresNeverDescriptor = modman_util::to_latin1("never");
    static constexpr QLatin1String s_deviceSerialDescriptor = modman_util::to_latin1("deviceSerial");
    static constexpr QLatin1String s_universalLicenseDescriptor = modman_util::to_latin1("universalLicense");
    static constexpr QLatin1String s_universalSerialDescriptor = modman_util::to_latin1("universalSerial");

    // EventSystem interface
public:
    bool processEvent(QEvent *t_event) override;
};

#endif // LICENSESYSTEM_H
