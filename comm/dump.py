import serial
import serial_datagram
import sys
import os
import msgpack
from pprint import pprint


def main():
    if len(sys.argv) > 2:
        baud = sys.argv[2]
    else:
        baud = 115200

    if len(sys.argv) == 1:
        fdesc = os.fdopen(0, "rb")
    else:
        fdesc = serial.Serial(sys.argv[1], baudrate=baud)

    while True:
        try:
            dtgrm = serial_datagram.read(fdesc)
            data = msgpack.unpackb(dtgrm)
            pprint(data)

        except (serial_datagram.CRCMismatchError, serial_datagram.FrameError):
            sys.stderr.write("CRC error")


if __name__ == '__main__':
    main()
