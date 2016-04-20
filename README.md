Epuck Firmware
==============
Firmware for the Epuck 2

### Cloning the repository

As this repository contains Git submodules you cannot use it right after cloning: You need to import the submodules first.
To do it run the following commands from the root of the project after the initial clone:

```bash
git submodule init
git submodule update
```

### Installing required Python tools

To use the tools in this repository you will need to install Python 3.
You can then install the requirements by running `pip3 install -r requirements.txt` from the root directory.
We recommend you install the tools in a [virtual environment][virtualenv] to avoid cluttering your system.


### Quickstart
Make sure you have an ARM GCC toolchain and OpenOCD installed.

```bash
make
make flash
```

If you get messages like `Fatal error: can't create build/obj/chcoreasm_v7m.o: No such file or directory`, run `make -B` instead of simply `make`.

To start the shell, open a terminal emulator and run

```bash
sudo python -m serial.tools.miniterm /dev/ttyACM0
```
 assuming `/dev/ttyACM0` is where the epuck is connected

[virtualenv]: http://docs.python-guide.org/en/latest/dev/virtualenvs/
