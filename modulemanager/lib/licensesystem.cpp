#include "licensesystem.h"

#include <vcmp_componentdata.h>
#include <ve_commandevent.h>
#include <ve_eventdata.h>

#include <vcb_opensslsignaturehandler.h>
QT_BEGIN_NAMESPACE
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
QT_END_NAMESPACE


static const char* s_systemNameDescriptor = "uniqueSystemName";
static const char* s_expiresDescriptor = "expires";
static const char* s_expiresNeverDescriptor = "never";
static const char* s_deviceSerialDescriptor = "deviceSerial";
static const char* s_universalLicenseDescriptor = "universalLicense";
static const char* s_universalSerialDescriptor = "universalSerial";


LicenseSystem::LicenseSystem(const QSet<QUrl> &t_licenseURLs, QObject *parent) : LicenseSystemInterface(parent),
    m_licenseURLs(t_licenseURLs),
    m_certData(loadCertData()),
    m_deviceSerial(QString("SERIAL NUMBER NOT FOUND!"))
{
    VeinCryptoBridge::OpenSSLSignatureHandler sigHandler;
    for(const QUrl &licenseUrl : m_licenseURLs)
    {
        ///@todo implement network requests for license files if required
        Q_ASSERT(licenseUrl.isLocalFile());

        const QHash<QString, QByteArray> licenseTable = getLicenseFilesFromPath(licenseUrl.toLocalFile());

        for(const QString &licenseFilePath : licenseTable.keys()) {
            const QByteArray licenseFileData = licenseTable.value(licenseFilePath);
            bool isVerified = false;
            QByteArray licenseJsonData;
            licenseJsonData = sigHandler.verifyCMSSignature(m_certData, licenseFileData, &isVerified);
            if(isVerified) {
                QJsonParseError parseError;
                QJsonDocument licenseDocument = QJsonDocument::fromJson(licenseJsonData, &parseError);
                if(parseError.error == QJsonParseError::NoError) {
                    const QJsonObject licenseRootObject = licenseDocument.object();
                    Q_ASSERT(licenseRootObject.contains(s_expiresDescriptor));
                    Q_ASSERT(licenseRootObject.contains(s_deviceSerialDescriptor));
                    const bool isUniversalLicense = (licenseRootObject.contains(s_universalLicenseDescriptor) && licenseRootObject.value(s_universalLicenseDescriptor).toInt(0) == 1);

                    if(isUniversalLicense
                            && isValidLicenseExpiryDate(licenseRootObject.value(s_expiresDescriptor).toString())
                            && isValidLicenseDeviceSerial(licenseRootObject.value(s_deviceSerialDescriptor).toString()))
                        m_universalLicenseFound = true;
                    Q_ASSERT(isUniversalLicense || licenseRootObject.contains(s_systemNameDescriptor));
                    if(licenseRootObject.contains(s_systemNameDescriptor))
                        m_verifiedLicenseDataTable.insert(licenseRootObject.value(s_systemNameDescriptor).toString(), licenseRootObject.toVariantMap());
                }
                else
                    qWarning() << "Error parsing license document:" << licenseFilePath << "\n" << "parse error:" << parseError.errorString();
            }
        }
    }
}

bool LicenseSystem::isSystemLicensed(const QString &t_uniqueSystemName)
{
    bool retVal = false;
    if(m_universalLicenseFound == true)
        retVal = true;
    else if(m_licensedSystems.contains(t_uniqueSystemName)) //known valid license
        retVal = true;
    else {
        const QVariantMap licenseRootObject = m_verifiedLicenseDataTable.value(t_uniqueSystemName);
        const QString licenseDeviceSerial = licenseRootObject.value(s_deviceSerialDescriptor).toString();
        if(isValidLicenseDeviceSerial(licenseDeviceSerial)) {
            const QString expiryMonth = licenseRootObject.value(s_expiresDescriptor).toString();
            if(isValidLicenseExpiryDate(expiryMonth)) {
                retVal = true;
                m_licensedSystems.append(licenseRootObject.value(s_systemNameDescriptor).toString());
            }
            else
                qWarning() << "License expired for system:" << t_uniqueSystemName << "\n" << "date:" << licenseRootObject.value(s_expiresDescriptor).toString();
        }
    }
    return retVal;
}

QVariantMap LicenseSystem::systemLicenseConfiguration(const QString &t_systemName) const
{
    return m_verifiedLicenseDataTable.value(t_systemName);
}

void LicenseSystem::setDeviceSerial(const QString &serialNumber)
{
    if(serialNumber.isEmpty() == false) {
        m_deviceSerial = serialNumber;
        m_serialNumberInitialized = true;
        emit sigSerialNumberInitialized();
    }
}

bool LicenseSystem::serialNumberIsInitialized() const
{
    return m_serialNumberInitialized;
}

QByteArray LicenseSystem::loadCertData() const
{
    QByteArray retVal;
    QFile certFile(":/license_cert.pem"); //do not use paths from the regular filesystem, with a replaced custom license_cert.pem licenses could be forged
    Q_ASSERT(certFile.exists());

    certFile.open(QFile::ReadOnly);
    retVal = certFile.readAll();
    certFile.close();

    return retVal;
}

QByteArray LicenseSystem::loadLicenseFile(const QString &filePath) const
{
    QByteArray retVal;
    QFile tmpFile(filePath);
    if(tmpFile.exists()) {
        QFileInfo tmpFileInfo(tmpFile);
        //mimetype is text/plain so use file extension do distinguish licenses from text files
        if(tmpFileInfo.completeSuffix().toLower() == "p7m") { //format: pkcs7 s/mime (Content-Type: multipart/signed; protocol="application/x-pkcs7-signature";)
            tmpFile.open(QFile::ReadOnly);
            retVal = tmpFile.readAll();
            tmpFile.close();
        }
        else
            qDebug() << "Ignored regular file:" << filePath << "(no .p7m extension)";
    }
    return retVal;
}

QHash<QString, QByteArray> LicenseSystem::getLicenseFilesFromPath(const QString &t_path) const
{
    QFileInfo fInfo(t_path);
    QHash<QString, QByteArray> retVal;

    if(fInfo.exists())
    {
        if(fInfo.isDir())
        {
            //recursive, only lists files
            QDirIterator tmpDirIterator(t_path, QDir::Files, QDirIterator::Subdirectories);
            while(tmpDirIterator.hasNext())
            {
                const QString nextFile = tmpDirIterator.next();
                const QByteArray licData = loadLicenseFile(nextFile);
                if(licData.isNull() == false)
                {
                    retVal.insert(nextFile, licData);
                }
            }
        }
        else if(fInfo.isFile())
        {
            const QByteArray licData = loadLicenseFile(t_path);
            if(licData.isNull() == false)
            {
                retVal.insert(t_path, licData);
            }
        }
    }
    else
    {
        qWarning() << "Could not load license info from path:" << t_path;
    }

    return retVal;
}

bool LicenseSystem::isValidLicenseExpiryDate(const QString t_dateString) const
{
    bool retVal = false;

    if(t_dateString == s_expiresNeverDescriptor)
    {
        retVal = true;
    }
    else
    {
        //the license expires at the end of the given month so add 1 month here
        const QDateTime tmpExpiryDate = QDateTime::fromString(t_dateString, "yyyy/MM").addMonths(1);
        if(tmpExpiryDate.isValid() && tmpExpiryDate.isNull() == false && tmpExpiryDate >= QDateTime::currentDateTime())
        {
            retVal = true;
        }
    }

    return retVal;
}

bool LicenseSystem::isValidLicenseDeviceSerial(const QString t_deviceSerial) const
{
    return (t_deviceSerial == s_universalSerialDescriptor || t_deviceSerial == m_deviceSerial);
}

void LicenseSystem::processEvent(QEvent *t_event)
{
    if(t_event->type()==VeinEvent::CommandEvent::eventType())
    {
        VeinEvent::CommandEvent *cEvent = static_cast<VeinEvent::CommandEvent *>(t_event);
        Q_ASSERT(cEvent != nullptr);

        VeinEvent::EventData *evData = cEvent->eventData();
        Q_ASSERT(evData != nullptr);

        //statusmodule initializes PAR_SerialNr with our serial number to check the licenses against
        if(evData->entityId() == 1150
                && evData->type() == VeinComponent::ComponentData::dataType()
                && evData->eventOrigin() == VeinEvent::EventData::EventOrigin::EO_LOCAL) {
            VeinComponent::ComponentData *cData = static_cast<VeinComponent::ComponentData *>(evData);
            if(cData->componentName() == QString("PAR_SerialNr")) {
                if(cData->eventCommand() == VeinComponent::ComponentData::Command::CCMD_ADD || cData->eventCommand() == VeinComponent::ComponentData::Command::CCMD_SET) {
                    const QString newSerialNumber = cData->newValue().toString();
                    if((newSerialNumber.isEmpty() == false) && (m_deviceSerial != newSerialNumber))
                    {
                        qWarning() << "Changed device serial from:" << m_deviceSerial << "to:" << cData->newValue() << cData->oldValue();
                        setDeviceSerial(newSerialNumber);
                    }
                }
            }
        }
    }
}
