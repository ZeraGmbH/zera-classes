zera-mcontroller-io: a command line tool to access ZERA µCs by i2c
-----------------------------------------------------------------------------------------------

Use `zera-mcontroller-io --help` to get available options:
```
zera-mcontroller-io --help
Usage: zera-mcontroller-io [options]
zera-mcontroller-io

Options:
  -h, --help                                   Displays this help.
  -v, --verbose <flags>                        1: More verbose syslog
  -i, --i2c-devnode <filename with full path>  I2C device node e.g '/dev/i2c-3'
  -I, --i2c-address <hex address>              I2C address (hex) e.g '20'
  -c, --cmd-id <hex command id>                Command ID (hex) e.g '0001'
  -d, --device <hex device no>                 Logical device (hex) e.g '01' -
                                               if not set: bootloader cmd
  -p, --cmd-param <hex param>                  Command parameter (hex) e.g
                                               '01AA' or '0x01 0xAA'
  -l, --return-len <expected len>              Expected data return length
                                               (decimal / without CRC / default:
                                               0 / variable x)
  -a, --convert-data-ascii <data-convert>      0: ouput data hex / 1: try to
                                               convert received data to ASCII /
                                               default 0
  -A, --convert-param-ascii <param-convert>    0: hex param e.g '0x01 0xA5' /
                                               1: interpret param data as ASCII
                                               / 2: interpret param data as
                                               ASCII + append trailing 0-byte /
                                               default 0
  -f, --flash-filename-write <hex filename>    Write hex file to flash
  -e, --eeprom-filename-write <hex filename>   Write hex file to eeprom
  -F, --flash-filename-verify <hex filename>   Verify hex file with flash
                                               (bootloader must support read
                                               commands)
  -E, --eeprom-filename-verify <hex filename>  Verify hex file with eeprom
                                               (bootloader must support read
                                               commands)
  -m, --max-write-block-count <max tries>      Maximum memory block writes in
                                               case of write error [1..255] /
                                               default: 2
```


Controller command examples for MT310s2
-------------------------------------------------------------

* Get Serialnumber length: Exec cmd but do not read data to determine data length
  ```sh
  zera-mcontroller-io -i/dev/i2c-0 -I21 -d0 -c1
  ```

* Read serialnumber: Read data by cmd with known data length
  ```sh
  zera-mcontroller-io -i/dev/i2c-0 -I21 -d0 -c1 -l9
  ```

* Read serialnumber: Read data by cmd with unknown data length / convert result to ASCII
  ```sh
  zera-mcontroller-io -i/dev/i2c-0 -I21 -d0 -c1 -lx -a1
  ```

* Write serialnumber: Cmd with parameter / no data returned
  ```sh
  zera-mcontroller-io -i/dev/i2c-0 -I21 -d0 -c6 -p'0x30 0x35 0x30 0x30 0x35 0x39 0x34 0x37 0x39'
  ```
  or as ASCII
  ```sh
  zera-mcontroller-io -i/dev/i2c-0 -I21 -d0 -c6 -A1 -p050059479
  ```

* Start bootloader: Cmd without parameter / no data returned
  __IMPORTANT NOTE__: When started by command, bootloader does not terminate  on timeout - it must be killed by command
  ```sh
  zera-mcontroller-io -i/dev/i2c-0 -I21 -d0 -c8
  ```


Bootloader command examples (all devices - hopefully)
-----------------------------------------------------
  __IMPORTANT NOTE__:
Old implementations of bootloader (all COM5003 shipped with release V2.x) require __exact__ sequence of commands -
otherwise it stops processing further commands. 'Exact' means:
* Do not read when bootloader waits for command
* If a command supplies data to return, this __data must be read once only__

Working Examples:
* Get Bootloader info len / data
  ```sh
  BL_INFO_LEN=`zera-mcontroller-io -i/dev/i2c-0 -I21 -c0 | sed 's|^.*can return data bytes: ||g'`
  BL_INFO=`zera-mcontroller-io -i/dev/i2c-0 -I21 -l$BL_INFO_LEN`
  echo $BL_INFO
  ```
  or more simple with '-lx' and return bootloader version as ASCII
  ```sh
  zera-mcontroller-io -i/dev/i2c-0 -I21 -c0 -lx -a1
  ```

* Write data to flash
  ```sh
  zera-mcontroller-io -i/dev/i2c-0 -I21 -f<hex-file-to-write>
  ```

* Verify data in flash
  ```sh
  zera-mcontroller-io -i/dev/i2c-0 -I21 -F<hex-file-to-write>
  ```

* Stop bootloader and start application
  ```sh
  zera-mcontroller-io -i/dev/i2c-0 -I21 -c1
  ```
