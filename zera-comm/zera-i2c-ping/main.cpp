#include <i2cutils.h>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationVersion("0.0.1");

    QCommandLineParser parser;
    parser.setApplicationDescription("zera-i2c-ping");
    parser.addHelpOption();
    // option for validation check of intel-hex-files
    QCommandLineOption cmdLineI2cDev(QStringList() << "d" << "i2c-device", "I2c device filename e.g /dev/i2c-0", "i2c-filename");
    parser.addOption(cmdLineI2cDev);
    QCommandLineOption cmdLineI2cAddr(QStringList() << "a" << "i2c-address", "I2c address (decimal)", "i2c-address");
    parser.addOption(cmdLineI2cAddr);

    parser.process(app);

    bool allOptsOk = true;

    QString i2cFileName = parser.value(cmdLineI2cDev);
    if(i2cFileName.isEmpty()) {
        allOptsOk = false;
        qWarning("I2cFile not set!\n");
    }
    else if(!QFile::exists(i2cFileName)) {
        allOptsOk = false;
        qWarning("I2cFile '%s' does not exist\n", qPrintable(i2cFileName));
    }

    QString i2cAddressStr = parser.value(cmdLineI2cAddr);
    if(i2cAddressStr.isEmpty()) {
        allOptsOk = false;
        qWarning("I2c address not set!\n");
    }
    int i2cAddress = i2cAddressStr.toInt();
    int i2cAddressMin = 0;
    int i2cAddressMax = 127;
    if(i2cAddress < i2cAddressMin || i2cAddress > i2cAddressMax) {
        allOptsOk = false;
        qWarning("I2c address %i out of limits [0-127]!\n", i2cAddress);
    }

    if(!allOptsOk) {
        // This exits application
        parser.showHelp(-1);
    }

    return I2cPing(i2cFileName, i2cAddress) ? 0 : -1;
}

