#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QRegularExpression>
#include <zera_mcontroller_base.h>

// Type of commands performed
static enum {
    CMD_UNDEF,
    CMD_BOOTLOADER_IO,
    CMD_BOOTLOADER_WRITE,
    CMD_ZERA_HARD_IO,
    CMD_READ_DATA
} cmdType = CMD_UNDEF;

// All data extracted from command line params
struct CommandLineData {
    bool verboseOutput = false;
    QString i2cDeviceName;
    quint8 i2cAddr = 0;
    QByteArray paramData;
    quint8 cmdIdBoot = 0;
    quint16 cmdIdHard = 0;
    quint8 cmdHardDevice = 0;
    quint16 cmdResponseLen = 0;
    cIntelHexFileIO flashHexData;
    cIntelHexFileIO eepromHexData;
};

/**
 * @brief convertCmdParam: Convert hexadecimal command param sting to binary data
 * @param paramValue [IN]: hexadecimal parameter string
 * @param binaryData [OUT]: binary parameter data
 * @return true on success
 */
static bool convertCmdParam(QString paramValue, QByteArray& binaryData)
{
    bool ok = true;
    paramValue.replace(" ", "");
    paramValue = paramValue.toUpper();
    paramValue.replace("0X", "");
    if(!QRegularExpression("^[0-9A-F]+$").match(paramValue).hasMatch()) {
        qWarning("%s is not a valid hexadecimal parameter!", qPrintable(paramValue));
        ok = false;
    }
    if(paramValue.size() % 1) {
        qWarning("%s has odd length %i!", qPrintable(paramValue), paramValue.size());
        ok = false;
    }
    if(ok) {
        binaryData.clear();
        for(int byte=0; byte<paramValue.size()/2; ++byte) {
            QString strByte = paramValue.mid(byte*2, 2);
            binaryData.append(static_cast<char>(strByte.toInt(nullptr, 16)));
        }
    }
    return ok;
}

/**
 * @brief parseCommandLine: Parse / check commandline params and fill our variables
 * @param coreApp: pointer to our application object
 * @param parser: pointer to QCommandLineParser object
 * @param cmdLineData: Data extracted from command line parameters
 * @return true on success
 */
static bool parseCommandLine(QCoreApplication* coreApp, QCommandLineParser *parser, CommandLineData *cmdLineData)
{
    parser->setApplicationDescription("zera-mcontroller-io");
    parser->addHelpOption();

    ////////////////////////////////////////////////////////
    // define command line param options

    // option for verbosity
    QCommandLineOption verboseOption(QStringList() << "v" << "verbose", "Debug output flags Bit0: error Bit1 data [0..3]", "flags");
    verboseOption.setDefaultValue("1");
    parser->addOption(verboseOption);
    // option for i2c device node
    QCommandLineOption i2cDevNodeOption(QStringList() << "i" << "i2c-devnode", "I2C device node e.g '/dev/i2c-3'", "filename with full path");
    parser->addOption(i2cDevNodeOption);
    // option for i2c address
    QCommandLineOption i2cAddressOption(QStringList() << "I" << "i2c-address", "I2C address (hex) e.g '20'", "hex address");
    parser->addOption(i2cAddressOption);
    // option for cmd id
    QCommandLineOption cmdIdOption(QStringList() << "c" << "cmd-id", "Command ID (hex) e.g '0001'", "hex command id");
    parser->addOption(cmdIdOption);
    // option for device num
    QCommandLineOption cmdDeviceNumOption(QStringList() << "d" << "device", "Logical device (hex) e.g '01' - if not set: bootloader cmd", "hex device no");
    parser->addOption(cmdDeviceNumOption);
    // option for param
    QCommandLineOption cmdParamOption(QStringList() << "p" << "cmd-param", "Command parameter (hex) e.g '01AA' or '0x01 0xAA'", "hex param");
    parser->addOption(cmdParamOption);
    // option for expected return data len (without crc)
    QCommandLineOption cmdReturnedLenOption(QStringList() << "l" << "return-len", "Expected data return length (decimal / without CRC / default: 0)", "expected len");
    parser->addOption(cmdReturnedLenOption);
    // option for bootloader write flash
    QCommandLineOption cmdFlashWriteOption(QStringList() << "f" << "flash-filename", "Write intel-hex file to flash", "hex filename");
    parser->addOption(cmdFlashWriteOption);
    // option for bootloader write eeprom
    QCommandLineOption cmdEepromWriteOption(QStringList() << "e" << "eeprom-filename", "Write intel-hex file to eeprom", "hex filename");
    parser->addOption(cmdEepromWriteOption);

    parser->process(*coreApp);

    bool allOptsOK = true;

    // Temp helpers
    bool optOK;
    QString optVal;
    int iFullVal;

    ////////////////////////////////////////////////////////
    // Check and parse cmdline into internal data

    ////////////////////////////////////////////////////////
    // Common options for all activities

    // verbosity
    optVal = parser->value(verboseOption);
    iFullVal = optVal.toInt(&optOK);
    if(!optOK || iFullVal<0 || iFullVal>1) {
        qWarning("Invalid value for verbose level %s!", qPrintable(optVal));
        allOptsOK = false;
    }
    else {
        cmdLineData->verboseOutput = iFullVal != 0;
    }
    // i2c-dev
    cmdLineData->i2cDeviceName = parser->value(i2cDevNodeOption);
    if(cmdLineData->i2cDeviceName.isEmpty()) {
        qWarning("No i2c device set!");
        allOptsOK = false;
    }
    else if(!QFile::exists(cmdLineData->i2cDeviceName)) {
        qWarning("I2c device does not %s exist!", qPrintable(cmdLineData->i2cDeviceName));
        allOptsOK = false;
    }
    // i2c-addr (we assume 7bit)
    optVal = parser->value(i2cAddressOption);
    if(optVal.isEmpty()) {
        qWarning("No i2c address set!");
        allOptsOK = false;
    }
    else  {
        iFullVal = optVal.toInt(&optOK, 16);
        if(!optOK || iFullVal<0 || iFullVal>0x7F) {
            qWarning("Invalid i2c-address %s!", qPrintable(optVal));
            allOptsOK = false;
        }
        else {
            cmdLineData->i2cAddr = static_cast<quint8>(iFullVal);
        }
    }

    ////////////////////////////////////////////////////////
    // No cmdID: -> read data from previous
    if(parser->value(cmdIdOption).isEmpty()) {
        cmdType = CMD_READ_DATA;
        // expected length of responded data
        optVal = parser->value(cmdReturnedLenOption);
        if(!optVal.isEmpty()) {
            int iFullVal = optVal.toInt(&optOK, 10);
            if(!optOK || iFullVal<1 || iFullVal>0xFFFF) {
                qWarning("Expected length %s for read is invalid or out of limits [1-65536]!", qPrintable(optVal));
                allOptsOK = false;
            }
            else {
                cmdLineData->cmdResponseLen = static_cast<quint16>(iFullVal);
            }
        }
    }

    ////////////////////////////////////////////////////////
    // With cmdID: -> IO
    //
    // No device set -> bootloader cmd
    else if(parser->value(cmdDeviceNumOption).isEmpty()) {
        // write flash?
        optVal = parser->value(cmdFlashWriteOption);
        if(!optVal.isEmpty()) {
            cmdType = CMD_BOOTLOADER_WRITE;
            if(!QFile::exists(optVal)) {
                qWarning("Flash file does not %s exist!", qPrintable(optVal));
                allOptsOK = false;
            }
            else if(!cmdLineData->flashHexData.ReadHexFile(optVal)) {
                qWarning("Flash file %s could not be read/converted!", qPrintable(optVal));
                allOptsOK = false;
            }
        }
        // write eeprom?
        optVal = parser->value(cmdEepromWriteOption);
        if(!optVal.isEmpty()) {
            cmdType = CMD_BOOTLOADER_WRITE;
            if(!QFile::exists(optVal)) {
                qWarning("EEPROM file does not %s exist!", qPrintable(optVal));
                allOptsOK = false;
            }
            else if(!cmdLineData->eepromHexData.ReadHexFile(optVal)) {
                qWarning("EEPROM file %s could not be read/converted!", qPrintable(optVal));
                allOptsOK = false;
            }
        }
        // common bootloader command?
        if(cmdType != CMD_BOOTLOADER_WRITE) {
            cmdType = CMD_BOOTLOADER_IO;
            // cmdID
            optVal = parser->value(cmdIdOption);
            int iFullVal = optVal.toInt(&optOK, 16);
            if(!optOK || iFullVal<0 || iFullVal>0xFF) {
                qWarning("Bootloader cmd ID %s is invalid or out of limits [0x00-0xFF]!", qPrintable(optVal));
                allOptsOK = false;
            }
            else {
                cmdLineData->cmdIdBoot = static_cast<quint8>(iFullVal);
            }
            // cmd param
            if(!parser->value(cmdParamOption).isEmpty()) {
                if(!convertCmdParam(parser->value(cmdParamOption), cmdLineData->paramData)) {
                    allOptsOK = false;
                }
            }
            // expected length of responded data
            optVal = parser->value(cmdReturnedLenOption);
            if(!optVal.isEmpty()) {
                int iFullVal = optVal.toInt(&optOK, 10);
                if(!optOK || iFullVal<0 || iFullVal>0xFFFF) {
                    qWarning("Expected length %s for bootloader cmd is invalid or out of limits [0-65535]!", qPrintable(optVal));
                    allOptsOK = false;
                }
                else {
                    cmdLineData->cmdResponseLen = static_cast<quint16>(iFullVal);
                }
            }
            else {
                // We have to set default here: If a default value is set in
                // cmdReturnedLenOption our cross plausi check below would fail
                cmdLineData->cmdResponseLen = 0;
            }
        }
    }
    ////////////////////////////////////////////////////////
    // With cmdID: -> IO
    //
    // device set -> hardware command
    else
    {
        cmdType = CMD_ZERA_HARD_IO;
        // cmdID
        optVal = parser->value(cmdIdOption);
        int iFullVal = optVal.toInt(&optOK, 16);
        if(!optOK || iFullVal<0 || iFullVal>0xFFFF) {
            qWarning("Cmd ID %s is invalid or out of limits [0x0000-0xFFFF]!", qPrintable(optVal));
            allOptsOK = false;
        }
        else {
            cmdLineData->cmdIdHard = static_cast<quint16>(iFullVal);
        }
        // device no
        optVal = parser->value(cmdDeviceNumOption);
        iFullVal = optVal.toInt(&optOK, 16);
        if(!optOK || iFullVal<0 || iFullVal>0xFF) {
            qWarning("Cmd device %s is invalid or out of limits [0x00-0xFF]!", qPrintable(optVal));
            allOptsOK = false;
        }
        else {
            cmdLineData->cmdHardDevice = static_cast<quint8>(iFullVal);
        }

        // cmd param
        optVal = parser->value(cmdParamOption);
        if(!optVal.isEmpty()) {
            if(!convertCmdParam(optVal, cmdLineData->paramData)) {
                allOptsOK = false;
            }
        }
        // expected length of responded data
        optVal = parser->value(cmdReturnedLenOption);
        if(!optVal.isEmpty()) {
            int iFullVal = optVal.toInt(&optOK, 10);
            if(!optOK || iFullVal<0 || iFullVal>0xFFFF) {
                qWarning("Expected length %s for cmd is invalid or out of limits [0-65536]!", qPrintable(optVal));
                allOptsOK = false;
            }
            else {
                cmdLineData->cmdResponseLen = static_cast<quint16>(iFullVal);
            }
        }
    }

    // check cross plausis
    switch(cmdType) {
    case CMD_UNDEF:
        qWarning("parseCommandLine(): Command type is undefined. This should never happen!");
        allOptsOK = false;
        break;
    case CMD_BOOTLOADER_WRITE:
        if(!parser->value(cmdIdOption).isEmpty()) {
            qWarning("Setting command id for bootloader file write is not allowed!");
            allOptsOK = false;
        }
        if(!parser->value(cmdParamOption).isEmpty()) {
            qWarning("Setting parameter data for bootloader file write is not allowed!");
            allOptsOK = false;
        }
        if(!parser->value(cmdReturnedLenOption).isEmpty()) {
            qWarning("Setting expected returned data length for bootloader file write is not allowed!");
            allOptsOK = false;
        }
        break;
    case CMD_READ_DATA:
        if(!parser->value(cmdParamOption).isEmpty()) {
            qWarning("Setting parameter data for read data is not allowed!");
            allOptsOK = false;
        }
        if(!parser->value(cmdDeviceNumOption).isEmpty()) {
            qWarning("Setting device number for read data is not allowed!");
            allOptsOK = false;
        }
        [[fallthrough]];
    case CMD_BOOTLOADER_IO:
    case CMD_ZERA_HARD_IO:
        if(!parser->value(cmdFlashWriteOption).isEmpty()) {
            qWarning("Setting flash file option for I/O command is not allowed!");
            allOptsOK = false;
        }
        if(!parser->value(cmdEepromWriteOption).isEmpty()) {
            qWarning("Setting eeprom file option for I/O command is not allowed!");
            allOptsOK = false;
        }
        break;
    }
    if(!allOptsOK) {
        // This exits application
        parser->showHelp(-1);
    }
    return allOptsOK;
}

/**
 * @brief outputReceivedData: If verboseOutput is set, output data to stdout
 * @param dataReceive: Buffer with data
 * @param receivedDataLen: Buffer length
 * @param cmdLineData: Data extracted from command line parameters
 */
static void outputReceivedData(quint8 *dataReceive, qint16 receivedDataLen, CommandLineData *cmdLineData)
{
    if(dataReceive && receivedDataLen > 1 && cmdLineData->verboseOutput) {
        QString dataString;
        // Do not output crc
        for(qint16 byteNo=0; byteNo<receivedDataLen-1; ++byteNo) {
            dataString += QString("0x%1 ").arg(dataReceive[byteNo], 2, 16, QLatin1Char('0'));
        }
        dataString = dataString.trimmed();
        qInfo("%s", qPrintable(dataString));
    }
}

/**
 * @brief execBootloaderIO: Execute bootloader command (data taken from cmdIdBoot/paramData/cmdResponseLen)
 * @param i2cController: pointer to ZeraMcontrollerBase object
 * @param cmdLineData: Data extracted from command line parameters
 * @return true on success
 */
static bool execBootloaderIO(ZeraMcontrollerBase* i2cController, CommandLineData *cmdLineData)
{
    bl_cmd bcmd(
                cmdLineData->cmdIdBoot,
                reinterpret_cast<quint8*>(cmdLineData->paramData.data()), static_cast<quint16>(cmdLineData->paramData.size()));
    quint8 *dataReceive = nullptr;
    quint16 totalReceiveLen = 0;
    if(cmdLineData->cmdResponseLen) {
        totalReceiveLen = cmdLineData->cmdResponseLen+1;
        dataReceive = new quint8[totalReceiveLen];
    }
    qint16 receivedDataLen = i2cController->writeBootloaderCommand(&bcmd, dataReceive, totalReceiveLen);
    outputReceivedData(dataReceive, receivedDataLen, cmdLineData);

    if(cmdLineData->cmdResponseLen == 0 && receivedDataLen > 1) {
        qInfo("bootcmd %02X can return data bytes (without CRC): %i ", cmdLineData->cmdIdBoot, receivedDataLen-1);
    }

    delete dataReceive;
    return (cmdLineData->cmdResponseLen==0 && receivedDataLen>=0) || totalReceiveLen == receivedDataLen;
}

/**
 * @brief execZeraHardIO: Execute hardware command (data taken from cmdIdHard/paramData/cmdResponseLen)
 * @param i2cController: pointer to ZeraMcontrollerBase object
 * @param cmdLineData: Data extracted from command line parameters
 * @return true on success
 */
static bool execZeraHardIO(ZeraMcontrollerBase* i2cController, CommandLineData *cmdLineData)
{
    hw_cmd hcmd(
                cmdLineData->cmdIdHard,
                cmdLineData->cmdHardDevice,
                reinterpret_cast<quint8*>(cmdLineData->paramData.data()), static_cast<quint16>(cmdLineData->paramData.size()));
    quint8 *dataReceive = nullptr;
    quint16 totalReceiveLen = 0;
    if(cmdLineData->cmdResponseLen) {
        totalReceiveLen = cmdLineData->cmdResponseLen+1;
        dataReceive = new quint8[totalReceiveLen];
    }
    qint16 receivedDataLen = i2cController->writeCommand(&hcmd, dataReceive, totalReceiveLen);
    outputReceivedData(dataReceive, receivedDataLen, cmdLineData);
    if(cmdLineData->cmdResponseLen == 0 && receivedDataLen > 1) {
        qInfo("cmd %04X can return data bytes (without CRC): %i", cmdLineData->cmdIdHard, receivedDataLen-1);
    }

    delete dataReceive;
    return (cmdLineData->cmdResponseLen==0 && receivedDataLen>=0) || totalReceiveLen == receivedDataLen;
}

/**
 * @brief execReadData: Read data from previous command (for cmds with unknown length len taken from cmdResponseLen)
 * @param i2cController: pointer to ZeraMcontrollerBase object
 * @param cmdLineData: Data extracted from command line parameters
 * @return true on success
 */
static bool execReadData(ZeraMcontrollerBase* i2cController, CommandLineData *cmdLineData)
{
    quint8 *dataReceive = nullptr;
    quint16 totalReceiveLen = 0;
    if(cmdLineData->cmdResponseLen) {
        totalReceiveLen = cmdLineData->cmdResponseLen+1;
        dataReceive = new quint8[totalReceiveLen];
    }
    qint16 receivedDataLen = i2cController->readOutput(dataReceive, totalReceiveLen);
    outputReceivedData(dataReceive, receivedDataLen, cmdLineData);

    delete dataReceive;
    return totalReceiveLen == receivedDataLen;
}

/**
 * @brief execBootloaderWrite: Write data in flashHexData/eepromHexData
 * @param i2cController: pointer to ZeraMcontrollerBase object
 * @param cmdLineData: Data extracted from command line parameters
 * @return true on success
 */
static bool execBootloaderWrite(ZeraMcontrollerBase* i2cController, CommandLineData *cmdLineData)
{
    bool bAllOK = true;
    if(!cmdLineData->flashHexData.isEmpty()) {
        if(i2cController->loadFlash(cmdLineData->flashHexData) != ZeraMcontrollerBase::cmddone) {
            bAllOK = false;
        }
    }
    if(bAllOK && !cmdLineData->eepromHexData.isEmpty()) {
        if(i2cController->loadEEprom(cmdLineData->eepromHexData) != ZeraMcontrollerBase::cmddone) {
            bAllOK = false;
        }
    }
    return bAllOK;
}



int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationVersion("0.0.1");

    QCommandLineParser parser;
    CommandLineData *cmdLineData = new CommandLineData;

    bool ok = false;
    if(parseCommandLine(&app, &parser, cmdLineData)) {
        // We output errors ALWAYS
        ZeraMcontrollerBase i2cController(
                    cmdLineData->i2cDeviceName,
                    cmdLineData->i2cAddr,
                    cmdLineData->verboseOutput ? 3 : 1);
        quint16 errMaskHost = i2cController.getLastErrorMask() >> 16;
        quint16 errMaskMController = i2cController.getLastErrorMask() & 0xFFFF;
        switch(cmdType) {
        case CMD_BOOTLOADER_IO:
            ok = execBootloaderIO(&i2cController, cmdLineData);
            if(!ok) {
                qWarning("bootcmd failed / see journalctl for more details / host-mask 0x%04X / µC-mask 0x%04X",
                    errMaskHost,
                    errMaskMController);
            }
            break;
        case CMD_ZERA_HARD_IO:
            ok = execZeraHardIO(&i2cController, cmdLineData);
            if(!ok) {
                qWarning("cmd failed / see journalctl for more details / host-mask 0x%04X / µC-mask 0x%04X",
                    errMaskHost,
                    errMaskMController);
            }
            break;
        case CMD_READ_DATA:
            ok = execReadData(&i2cController, cmdLineData);
            if(!ok) {
                qWarning("read data failed / see journalctl for more details / host-mask 0x%04X / µC-mask 0x%04X",
                    errMaskHost,
                    errMaskMController);
            }
            break;
        case CMD_BOOTLOADER_WRITE:
            ok = execBootloaderWrite(&i2cController, cmdLineData);
            if(!ok) {
                qWarning("boot write failed / see journalctl for more details / host-mask 0x%04X / µC-mask 0x%04X",
                    errMaskHost,
                    errMaskMController);
            }
            break;
        case CMD_UNDEF:
            qWarning("main(): Command type is undefined. This should never happen!");
            break;
        }
    }
    delete cmdLineData;

    return ok ? 0 : -1;//app.exec();
}
