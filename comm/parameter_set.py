import serial
import serial_datagram
import yaml
import msgpack
import argparse
import os
import sys
import time
from pprint import pprint


def parameter_send(serial_port, parameter_file):
    parameters = yaml.load(parameter_file)
    pprint(parameters, sys.stderr)
    msg = msgpack.packb({'parameter_set': parameters}, use_single_float=True)  # todo pack as float
    dtgrm = serial_datagram.encode(msg)
    serial_port.write(dtgrm)
    serial_port.flush()


def main():
    parser = argparse.ArgumentParser("send parameters over serial datagram")
    parser.add_argument("file", help="YAML parameter file")
    parser.add_argument("--dev", help="serial port device")
    parser.add_argument("--baud", help="serial port baud rate", default=115200)
    parser.add_argument("-w", "--watch",
                        help="Watch parameter file for changes.",
                        action="store_true")
    args = parser.parse_args()

    if args.dev:
        fdesc = serial.Serial(args.dev, args.baud)
    else:
        fdesc = os.fdopen(1, 'wb')

    parameter_send(fdesc, open(args.file))

    if args.watch:
        # print("> watching for file changes...")
        old_mtime = os.path.getmtime(args.file)
        while True:
            try:
                try:
                    mtime = os.path.getmtime(args.file)
                # Some editors delete the file before writing to it
                except FileNotFoundError:
                    pass

                if mtime != old_mtime:
                    old_mtime = mtime
                    parameter_send(fdesc, open(args.file))

                time.sleep(0.1)
            except KeyboardInterrupt:
                break

if __name__ == '__main__':
    main()
