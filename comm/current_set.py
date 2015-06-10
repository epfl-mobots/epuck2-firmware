import serial
import serial_datagram
import yaml
import msgpack
import argparse
import os
import sys
import time
from pprint import pprint


def current_set(serial_port, current):
    msg = msgpack.packb({'current_set': [True, float(current)]},  use_single_float=True)
    dtgrm = serial_datagram.encode(msg)
    serial_port.write(dtgrm)
    serial_port.flush()


def main():
    parser = argparse.ArgumentParser("set current setpoint")
    parser.add_argument("dev", help="serial port device")
    parser.add_argument("--baud", help="serial port baud rate", default=115200)
    parser.add_argument("current", help="motor current")
    args = parser.parse_args()

    fdesc = serial.Serial(args.dev, args.baud)

    current_set(fdesc, args.current)

if __name__ == '__main__':
    main()
