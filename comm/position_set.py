import serial
import serial_datagram
import yaml
import msgpack
import argparse
import os
import sys
import time
from pprint import pprint


def position_set(serial_port, position):
    msg = msgpack.packb({'position_set': [True, float(position)]},  use_single_float=True)
    dtgrm = serial_datagram.encode(msg)
    serial_port.write(dtgrm)
    serial_port.flush()


def main():
    parser = argparse.ArgumentParser("set position setpoint")
    parser.add_argument("dev", help="serial port device")
    parser.add_argument("--baud", help="serial port baud rate", default=115200)
    parser.add_argument("position", help="motor position")
    args = parser.parse_args()

    fdesc = serial.Serial(args.dev, args.baud)

    position_set(fdesc, args.position)

if __name__ == '__main__':
    main()
