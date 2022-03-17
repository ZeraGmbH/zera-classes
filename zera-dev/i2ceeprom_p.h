#ifndef I2CEEPROM_P_H
#define I2CEEPROM_P_H

#include <QString>
#include <QTimer>

/**
  @brief virtual base class for i2c flash or eeprom devices
  */


class cI2CEEPromPrivate // virtuelle basisklasse für i2c eeproms
{
public:
    /**
      @b Initializes a new eeprom object
      @param[in] dNode is full path i2c device node /dev/i2c-3/0 for example
      @param[in] d(ebug)Level decides what to write to syslog
      @param[in] (i2c)adr is the devices adress
      */
    cI2CEEPromPrivate(QString dNode, int dLevel, ushort adr);
    cI2CEEPromPrivate(){};
    virtual ~cI2CEEPromPrivate(){};


    /** @brief Writes (count) bytes starting at (adr) data to the device
     * @param data is pointer
     * @param count is bytes to write
     * @param adr defines the start adress in the eeprom
     */
    virtual int WriteData(char* data, ushort count, ushort adr)=0; // schreibt die daten ab char*  , mit der länge (int) ab adresse (int) ins EEProm; rückgabewert gibt die tatsächlich geschriebenen bytes an
    /**
     * @brief Reads (count) bytes starting at (adr) data from the device
     * @param data is pointer
     * @param count count is bytes to read
     * @param adr defines the start adress in the eeprom
     */
    virtual int ReadData(char* data, ushort count, ushort adr)=0; // wie write, nur halt zum lesen
    virtual int Reset() = 0;
    /**
     * @brief returns the devices size in bytes
     */
    virtual int size()=0;
protected:
    QString DevNode;
    int DebugLevel;
    ushort I2CAdress;
};


#endif // I2CEEPROM_P_H
