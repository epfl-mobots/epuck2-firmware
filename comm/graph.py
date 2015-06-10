from pyqtgraph.Qt import QtGui, QtCore
import numpy as np
import pyqtgraph as pg
import collections
import argparse
import sys
import signal
import os
import serial_datagram
import msgpack
import serial


class PlotVar:
    def __init__(self, plot, color, name=None, maxhist=10000):
        self.name = name
        self.maxhist = maxhist
        self.buffer = collections.deque([0.0]*self.maxhist, self.maxhist)
        self.plot = plot.plot(name=name, pen=color, fillLevel=0, brush=color+'40')

    def updatePlot(self, histlen):
        self.plot.setData(self.buffer)

    def updateData(self, datapt):
        self.buffer.append(datapt)


class Plot:
    def __init__(self, window, histlen=10000):
        self.plt = window.addPlot()
        self.plt.addLegend()
        self.plt.showGrid(x=True, y=True)
        self.histlen = histlen
        self.maxhistlen = histlen
        self.lines = []
        self.plt.sigYRangeChanged.connect(lambda: self._updateRegionHanlder())

    def addLine(self, color, name=None):
        l = PlotVar(self.plt, color, name=name, maxhist=self.maxhistlen)
        self.lines.append(l)
        return l

    def update(self):
        # self.plt.setXRange(0, self.histlen, padding=0, update=False)
        # for l in self.lines:
            # l.updateData(np.random.normal())
        for l in self.lines:
            l.updatePlot(self.histlen)
        self.plt.enableAutoRange('y', True)

    def _updateRegionHanlder(self):
        d = self.plt.getViewBox().viewRange()[0]
        self.histlen = d[1] - d[0]
        if self.histlen > self.maxhistlen:
            self.histlen = self.maxhistlen


plot_colors = [
    '#2c3e50',
    '#c0392b',
    '#2980b9',
    '#8e44ad',
    '#16a085',
    '#f1c40f'
    ]


def variable_path_extract_value(variable, data):
    path = variable.split('/')
    for var in path:
        arr_var = var.split('.')
        var_key = arr_var.pop(0).encode('utf-8')
        if var_key in data:
            data = data[var_key]
            for idx in arr_var:
                if (int(idx) < len(data)):
                    data = data[int(idx)]
                else:
                    return None
        else:
            return None
    try:
        return float(data)
    except TypeError:
        return None

    if variable.encode('utf-8') in data:
        return data[variable.encode('utf-8')]
    return None


def main():
    parser = argparse.ArgumentParser("plot values from msgpack stream")
    parser.add_argument("--dev", help="serial port device")
    parser.add_argument("--baud", help="serial port baud rate", default=115200)
    parser.add_argument("--hist", help="length of the plot-history", type=int, default=1000)
    parser.add_argument("var", help="variable to plot", nargs='+')

    args = parser.parse_args()

    if args.dev:
        fdesc = serial.Serial(args.dev, args.baud)
    else:
        fdesc = os.fdopen(0, "rb")

    app = QtGui.QApplication([])

    pg.setConfigOption('background', '#EEEEEE')
    pg.setConfigOptions(antialias=True)
    win = pg.GraphicsWindow(title="e-puck: dataplot")

    plot = Plot(win, histlen=args.hist)
    for color_idx, var in enumerate(args.var):
        plot.addLine(plot_colors[color_idx % len(plot_colors)], var)

    timer = QtCore.QTimer()
    timer.timeout.connect(plot.update)
    timer.start(50)

    class DatagramRcv(QtCore.QThread):
        def __init__(self, fdesc, variables):
            self.fdesc = fdesc
            self.variables = variables
            super(DatagramRcv, self).__init__()

        def run(self):
            while True:
                try:
                    dtgrm = serial_datagram.read(self.fdesc)
                    data = msgpack.unpackb(dtgrm)
                    for idx, var in enumerate(self.variables):
                        val = variable_path_extract_value(var, data)
                        if val is not None:
                            plot.lines[idx].updateData(val)
                except (serial_datagram.CRCMismatchError, serial_datagram.FrameError):
                    print("CRC error")

    rcv_thread = DatagramRcv(fdesc, args.var)
    rcv_thread.start()

    signal.signal(signal.SIGINT, signal.SIG_DFL)  # to kill on ctl-C

    if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
        QtGui.QApplication.instance().exec_()
    rcv_thread.terminate()
    fdesc.close()

if __name__ == '__main__':
    main()
