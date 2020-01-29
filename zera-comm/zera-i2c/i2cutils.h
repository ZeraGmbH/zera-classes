#ifndef I2CUTILS_H
#define I2CUTILS_H

#include <QString>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "i2c_global.h"


/**
  @brief
  Transfers the i2c_rdwr_ioctl_data block to the desired device node

  @param[in] deviceNode is full path i2c device node /dev/i2c-3/0 for example
  @param[in] i2cadr is sent debug information if desired
  @param[in] debugLevel decides what to write to syslog
  */
int ZERAI2C_EXPORT I2CTransfer( QString deviceNode, int i2cadr, int debugLevel, i2c_rdwr_ioctl_data* iodata);


#endif // I2CUTILS_H
