Epuck Firmware
==============
Firmware for the Epuck 2

### Quickstart
Make sure you have an ARM GCC toolchain and OpenOCD installed.

```bash
git submodule init
git submodule update

make
make flash
```

To start the shell, open a terminal emulator and run

```bash
sudo python -m serial.tools.miniterm /dev/ttyACM0
```
 assuming `/dev/ttyACM0` is where the epuck is connected
