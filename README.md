Epuck Firmware
==============
Firmware for the Epuck 2

## Requirements

This project requires the following tools:

* A recent version of GCC/G++ for ARM (Tested with 4.9+)
* OpenOCD 0.9 or later
* A C / C++ compiler for the host (only required for unit tests)
* [Packager](packager), which is a tool to generate makefiles with dependencies.
    Once you installed Python 3, it can be installed by running `pip3 install cvra-packager`.

## Quickstart
Make sure you have an ARM GCC toolchain and OpenOCD installed.

```bash
git submodule init
git submodule update

packager

make
make flash
```

If you get messages like `Fatal error: can't create build/obj/chcoreasm_v7m.o: No such file or directory`, run `make -B` instead of simply `make`.

To start the shell, open a terminal emulator and run

```bash
sudo python -m serial.tools.miniterm /dev/ttyACM0
```
 assuming `/dev/ttyACM0` is where the robot is connected.

## Using IP networking

Using the IP network is currently only supported over USB.
This means that the shell won't be available if IP networking is enabled.

To use IP networking you need to compile the code with the `USE_SERIAL_IP` option enabled (i.e. `make USE_SERIAL_IP=yes`).
Then, you need to configure your computer to use the serial link as a network interface.
On Linux (you might need to run the following commands as root or with `sudo`):

```bash
# In a first terminal
slattach -v -p slip /dev/ttyACM0

# In another terminal
ifconfig sl0 192.168.3.1 netmask 255.255.255.255 pointopoint 192.168.3.2
ifconfig sl0 up

# Test the connection
ping 192.168.3.2 # epuck2 has ip 192.168.3.2
```


### Changing node ID
To change the node ID, open a shell to the device and enter the following commands:

```
config_set /aseba/id <new id>
config_save
```

then reset the board

### Running unit tests

When developping for this project you might want to run the unit tests to check that your work is still OK.
To do this you will need the following:

* A working C/C++ compiler
* CMake
* [Cpputest][cpputest] A C++ unit testing library

Once everything is installed you can run the following:

```
packager
mkdir build
cd build
cmake ..
make check
```

## Code organization

The code is split into several subsystems:

* `main.c` contains the entry point of the program.
    Its main role is to start all the services and threads required by the rest of the application.
* `chconf.h`, `halconf.h` and `mcuconf.h` are ChibiOS configuration files.
    They allow to customize OS behavior, add / remove device drivers, etc.
    1. `halconf.h` allows the user to activate and deactivate the different features of the Hardware Abstraction Layer (HAL).
    2. `mcuconf.h` contains the low level details regarding the microcontroller (DMA streams, clocks, etc.).
        You should not have to change it.
    3. `chconf.h` allows the user to configure the different features of ChibiOS (minus the HAL).
        This is were the user can activate and deactivate features like events, mutexes and semaphores.
* `cmd.c` defines the commands available through the debug shell.
* `config_flash_storage.c` contains the code used to store the parameter tree to flash.
    It allows user settings to be persistent across reboots.
* `memory_protection.c` contains a driver for the Memory Protection Unit (MPU).
    In this project the MPU is used to detect basic bugs, such as NULL pointer dereference and jumping to invalid function pointers.
* `panic.c` contains the panic handler, called when the system crashes.
* `parameter_port.h` defines OS-specific locking mechanisms used by the parameter tree subsystem.
* `usbcfg.c` contains configuration strings for the USB port.
* `battery_protection.c` contains the code for the overdischarge protection (see below).

* The `sensors` folder contains the drivers used for the robot sensors:
    * `battery_level.c` is responsible for reading the battery voltage.
    * `encoder.c` handles the quadrature encoders of the motors and correctly adds them up to 32 bits numbers from their 16 bit hardware timer.
    * `imu.c` and `mpu60x0.c` contain the drivers for the inertial motion unit (gyro and accelerometer).
    * `proximity.c` contains the drivers for the TCRT1000-based proximity sensor belt.
    * `range.c` contains the interface for the Time of Flight range sensor.
        The low level driver is located in the `vl6180x` folder.
* `exti.c` acts as the central hub for the GPIO interrupts on the robot and dispatches them to the correct tasks.
    For now it is only used by the IMU to indicate when a measurement is ready.
* `motor_pwm.c` contains the code to drive the motors.
* The `lwip` folder contains the IP stack used for communication with a PC.
    It includes an IP over UART implementation (SLIP), but other medias could be added (Ethernet, CDC, Wifi, ...)

`aseba_vm` contains all the porting code to run Aseba on this platform.
* `skel_user.c` contains application-specific code, such as native functions, event definitions, etc.
* `aseba_node.c` contains the thread that runs the Aseba VM, as well as OS-specific callbacks.
* `aseba_can_interface.c` contains CAN drivers for use by Aseba.

The following modules are also used, see their respective documentation for more details:

* `chibios-syscalls` contains Newlib porting code and is required for standard library functions such as `printf (3)`, `malloc (3)`, etc.
* `cmp` & `cmp_mem_access` contain an implementation of the [MessagePack standard][messagepack] for C.
* `crc` is an implementation of the Cyclic Redundancy Check (CRC).
    CRC32 and CRC16 are included.
* `parameter` contains an implementation of a centralized parameter service which allows users to make their code configurable from a single place.
    See doc for details.
* `test-runner` does not contain code that will run on the robot but is required to run the unit tests.

### MessageBus
The code is usually organized around the principle of one thread per sensor/actuator/functionality.
Those threads communicate using a software message bus (publisher/subscriber pattern), similar in concept to D-bus or ROS.
This technology prevents the need for custom locking algorithm in user code, which in turns increase code correctness.
See `src/msgbus/README.md` for more information.

## Automated low battery cutoff
Over discharging a Lithium battery can damage it permanently.
In order to prevent that, the robot will warn the user via a blinking red LED when the battery goes below a certain threshold (3.5V/cell).
If the battery goes under a critical voltage (3V/cell), the robot self-shutdowns to protect the battery.
**However there are some situations in which this system will not protect your batteries**:
1. If the software is running in a step by step debugger, or
2. If the "Keep power enabled" jumper (JP2) is on, or
3. If software modifications result in the software being locked in a critical zone.
    This is an example of why critical zones should absolutely be avoided.
    The protection code runs at highest priority and the RTOS is configured to use round-robin scheduling so thread starvation should not happen.

## Automated code formatting
Running `make format` will reformat the code to follow the coding style more accurately.
Please do it before committing.

[cpputest]: http://cpputest.github.io
[packager]: http://github.com/cvra/packager
[messagepack]: http://messagepack.org/
