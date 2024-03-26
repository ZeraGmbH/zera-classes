#ifndef LICENSESYSTEM_H
#define LICENSESYSTEM_H

#include "licensesysteminterface.h"
#include <ve_eventsystem.h>
#include <QMap>
#include <QHash>
#include <QUrl>

class LicenseSystem : public LicenseSystemInterface
{
    Q_OBJECT
public:
    LicenseSystem(const QSet<QUrl> &t_licenseURLs, QObject *parent = nullptr);

    bool isSystemLicensed(const QString &uniqueModuleName) override;
    /**
   * @brief Returns the data of the license json objects for the given system
   * @param t_systemName
   * @return empty QVariantMap() if no system exists with t_systemName
   */
    QVariantMap systemLicenseConfiguration(const QString &t_systemName) const;

    void setDeviceSerial(const QString &serialNumber);
    /**
   * @brief serialNumberIsInitialized
   * @return true if the StatusModule1.PAR_SerialNr was retrieved
   */
    bool serialNumberIsInitialized() const override;

private:
    void processEvent(QEvent *t_event) override;
   /**
   * @brief loads the license data from ":/license_cert.pem"
   * @return
   */
    QByteArray loadCertData() const;
    /**
   * @brief reads a license file from the path
   * @param filePath
   * @return
   */
    QByteArray loadLicenseFile(const QString &filePath) const;
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

};

#endif // LICENSESYSTEM_H
