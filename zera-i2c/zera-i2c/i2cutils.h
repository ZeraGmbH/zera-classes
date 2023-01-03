#ifndef I2CUTILS_H
#define I2CUTILS_H

#include <zera-i2c_export.h>
#include <QString>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

/**
  @brief
  Transfers the i2c_rdwr_ioctl_data block to the desired device node

  @param[in] deviceNode is full path i2c device node /dev/i2c-3/0 for example
  @param[in] i2cadr is sent debug information if desired
  */
int ZERA_I2C_EXPORT I2CTransfer(QString deviceNode, int i2cadr, i2c_rdwr_ioctl_data* iodata);

bool ZERA_I2C_EXPORT I2cPing(QString deviceNode, int i2cadr);

#endif // I2CUTILS_H
