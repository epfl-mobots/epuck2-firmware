import serial
import serial_datagram
import yaml
import msgpack
import argparse
import os
import sys
import time
from pprint import pprint


def velocity_set(serial_port, speed):
    msg = msgpack.packb({'velocity_set': [True, float(speed)]},  use_single_float=True)
    dtgrm = serial_datagram.encode(msg)
    serial_port.write(dtgrm)
    serial_port.flush()


def main():
    parser = argparse.ArgumentParser("set velocity setpoint")
    parser.add_argument("dev", help="serial port device")
    parser.add_argument("--baud", help="serial port baud rate", default=115200)
    parser.add_argument("speed", help="motor speed")
    args = parser.parse_args()

    fdesc = serial.Serial(args.dev, args.baud)

    velocity_set(fdesc, args.speed)

if __name__ == '__main__':
    main()
