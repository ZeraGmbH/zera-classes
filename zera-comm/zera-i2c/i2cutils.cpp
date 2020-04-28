#include <syslog.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <QByteArray>
#include <errno.h>

#include "i2cutils.h"

int I2CTransfer(QString deviceNode,int i2cadr, int debugLevel, i2c_rdwr_ioctl_data* iodata)
{
    int fd;
    if ( (fd = open(deviceNode.toLatin1().data(),O_RDWR)) < 0 ) {
        if (debugLevel & 1) {
            syslog(LOG_ERR,"error opening i2c device: %s / open() errno %i\n",
                   deviceNode.toLatin1().data(), errno);
        }
        return(1); // error connection
    }
    if (ioctl(fd,I2C_RETRIES,0) < 0) {
        close(fd);
        if (debugLevel & 1) {
            syslog(LOG_ERR,"error setting retries of i2c device: %s / ioctl(fd,I2C_RETRIES,0) errno %i\n",
                   deviceNode.toLatin1().data(), errno);
        }
        return(1); // error connection
    }
    if (ioctl(fd,I2C_TIMEOUT,500) < 0) {
        close(fd);
        if (debugLevel & 1) {
            syslog(LOG_ERR,"error setting retries of i2c device: %s / ioctl(fd,I2C_TIMEOUT,500) errno %i\n",
                   deviceNode.toLatin1().data(), errno);
        }
        return(1); // error connection
    }
    if (ioctl(fd,I2C_RDWR,iodata) < 0) {
        if (debugLevel & 1) {
            syslog(LOG_ERR,"error read/write i2c slave at adress: %d / ioctl(fd,I2C_RDWR,iodata) errno %i\n",
                   i2cadr, errno);
        }
        close(fd);
        return(2); // error device
    }

    if (debugLevel & 2) {
        syslog(LOG_INFO,"read/write i2c slave at adress: %d done\n",
               i2cadr);
    }
    close(fd);
    return(0); // acknowledge
}
