#!/usr/bin/env python3
'''
import matplotlib.pyplot as plt
from drawnow import *

import numpy as np
import matplotlib.animation as animation

import serial

import time

r = []
p = []
y = []
ax = []
ay = []
az = []
kr = []
kp = []
ky = []
kax = []
kay = []
kaz = []

limit_val = 75

ser = 0

def update(data):
    global line
    line.set_ydata(data)
    return line,

def run(data):
  global xdata, ydata, line
  r,kr = data
  del xdata[0]
  del ydata[0]
  xdata.append(r)
  ydata.append(kr)
  line.set_data(xdata, ydata)
  return line,

def data_gen():
  global ser, line, t0
  dat = 0
  t = 0
  while True:
    t+=0.1
    try:
      raw = ser.readline()
      raw = raw[:len(raw)-2].decode("utf-8")
      lines = str(raw).split(',')
      if len(lines) is 12:
        cnt=0
        for i in range(len(raw)):
          if raw[i] is ',':
            cnt = cnt+1
        if cnt is 11:
          #print(raw)
          t1 = time.time()-t0
          print([t1, lines])
          t0 = time.time()

          r.append(float(line[0]))
          p.append(float(line[1]))
          y.append(float(line[2]))
          ax.append(float(line[3]))
          ay.append(float(line[4]))
          az.append(float(line[5]))

          kr.append(float(line[6]))
          kp.append(float(line[7]))
          ky.append(float(line[8]))
          kax.append(float(line[9]))
          kay.append(float(line[10]))
          kaz.append(float(line[11]))

          dat = float(lines[0])
    except:
      dat = 0
    yield t, dat


def main2():
  global ser, xdata, ydata, line, t0
  #main()
  
  t0 = time.time()

  ser = serial.Serial("/dev/ttyACM0",9600)
  while(1):
    xx = ser.readline()
    t1 = time.time()-t0
    print([t1, xx])
    t0 = time.time()
  
  exit()
  fig, ax = plt.subplots()
  line, = ax.plot(np.random.rand(10))
  ax.set_ylim(-10000, 10000)
  xdata, ydata = [0]*100, [0]*100

  ani = animation.FuncAnimation(fig, run, data_gen, interval=0, blit=True)
  plt.show()


if __name__ == '__main__':
  main2()
'''


#!/usr/bin/env python3
 
from threading import Thread
import serial
import time
import collections
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import struct
import copy
 
 
class serialPlot:
    def __init__(self, serialPort='/dev/ttyACM0', serialBaud=9600, plotLength=100, dataNumBytes=2, numPlots=1):
        self.port = serialPort
        self.baud = serialBaud
        self.plotMaxLength = plotLength
        self.dataNumBytes = dataNumBytes
        self.numPlots = numPlots
        self.rawData = bytearray(numPlots * dataNumBytes)
        self.dataType = None
        if dataNumBytes == 2:
            self.dataType = 'h'     # 2 byte integer
        elif dataNumBytes == 4:
            self.dataType = 'f'     # 4 byte float
        self.data = []
        self.privateData = None     # for storing a copy of the data so all plots are synchronized
        #for i in range(numPlots):   # give an array for each type of data and store them in a list
        for i in range(12):   # give an array for each type of data and store them in a list
            self.data.append(collections.deque([0] * plotLength, maxlen=plotLength))
        self.isRun = True
        self.isReceiving = False
        self.thread = None
        self.plotTimer = 0
        self.previousTimer = 0
 
        print('Trying to connect to: ' + str(serialPort) + ' at ' + str(serialBaud) + ' BAUD.')
        try:
            self.serialConnection = serial.Serial(serialPort, serialBaud, timeout=4)
            print('Connected to ' + str(serialPort) + ' at ' + str(serialBaud) + ' BAUD.')
        except:
            print("Failed to connect with " + str(serialPort) + ' at ' + str(serialBaud) + ' BAUD.')
 
    def readSerialStart(self):
        if self.thread == None:
            self.thread = Thread(target=self.backgroundThread)
            self.thread.start()
            # Block till we start receiving values
            while self.isReceiving != True:
                time.sleep(0.1)
 
    def getSerialData(self, frame, lines1, lines2, lineValueText, lineLabel, timeText, pltNumber):
        if pltNumber == 0:  # in order to make all the clocks show the same reading
            currentTimer = time.perf_counter()
            self.plotTimer = int((currentTimer - self.previousTimer) * 1000)     # the first reading will be erroneous
            self.previousTimer = currentTimer
        self.privateData = copy.deepcopy(self.rawData)    # so that the 3 values in our plots will be synchronized to the same sample time
        timeText.set_text('Plot Interval = ' + str(self.plotTimer) + 'ms')
        #data = self.privateData[(pltNumber*self.dataNumBytes):(self.dataNumBytes + pltNumber*self.dataNumBytes)]
        data = str(self.privateData[:len(self.privateData)-2].decode("utf-8")).split(',')
        print(data)
        #value,  = struct.unpack(self.dataType, data)
        value = [float(x) for x in data]
        self.data[pltNumber].append(value[pltNumber])    # we get the latest data point and append it to our array
        self.data[pltNumber+6].append(value[pltNumber+6])    # we get the latest data point and append it to our array
        lines1.set_data(range(self.plotMaxLength), self.data[pltNumber])
        lines2.set_data(range(self.plotMaxLength), self.data[pltNumber+6])
        lineValueText.set_text('[' + lineLabel + '] = ' + str(value))

    def backgroundThread(self):    # retrieve data
        time.sleep(1.0)  # give some buffer time for retrieving data
        self.serialConnection.reset_input_buffer()
        while (self.isRun):
            #self.serialConnection.readinto(self.rawData)
            self.rawData = self.serialConnection.readline()
            self.isReceiving = True
 
    def close(self):
        self.isRun = False
        self.thread.join()
        self.serialConnection.close()
        print('Disconnected...')
 
 
def makeFigure(xLimit, yLimit, title):
    xmin, xmax = xLimit
    ymin, ymax = yLimit
    fig = plt.figure()
    ax = plt.axes(xlim=(xmin, xmax), ylim=(int(ymin - (ymax - ymin) / 10), int(ymax + (ymax - ymin) / 10)))
    ax.set_title(title)
    ax.set_xlabel("Time")
    ax.set_ylabel("Output")
    return fig, ax
 
 
def main():
    # portName = 'COM5'
    portName = '/dev/ttyACM0'
    baudRate = 9600
    maxPlotLength = 100     # number of points in x-axis of real time plot
    dataNumBytes = 4        # number of bytes of 1 data point
    numPlots = 6            # number of plots in 1 graph
    s = serialPlot(portName, baudRate, maxPlotLength, dataNumBytes, numPlots)   # initializes all required variables
    s.readSerialStart()                                               # starts background thread
 
    # plotting starts below
    pltInterval = 50    # Period at which the plot animation updates [ms]
    lineLabelText = ['R', 'P', 'Y', 'AX', 'AY', 'AZ']
    title = ['Roll', 'Pitch', 'Yaw', 'X Acceleration', 'Y Acceleration', 'Z Acceleration']
    xLimit = [(0, maxPlotLength), (0, maxPlotLength), (0, maxPlotLength), (0, maxPlotLength), (0, maxPlotLength), (0, maxPlotLength)]
    yLimit = [(-100000, 100000), (-100000, 100000), (-100000, 100000), (-1000, 1000), (-1000, 1000), (-1000, 1000)]
    style = ['r-', 'g-', 'b-', 'r-', 'g-', 'b-']    # linestyles for the different plots
    anim = []
    for i in range(numPlots):
        fig, ax = makeFigure(xLimit[i], yLimit[i], title[i])
        lines1 = ax.plot([], [], 'r', style[i], label=lineLabelText[i])[0]
        lines2 = ax.plot([], [], 'g', style[i], label=lineLabelText[i])[0]
        timeText = ax.text(0.50, 0.95, '', transform=ax.transAxes)
        lineValueText = ax.text(0.50, 0.90, '', transform=ax.transAxes)
        anim.append(animation.FuncAnimation(fig, s.getSerialData, fargs=(lines1, lines2, lineValueText, lineLabelText[i], timeText, i), interval=pltInterval))  # fargs has to be a tuple
        plt.legend(loc="upper left")
    plt.show()
 
    s.close()
 
 
if __name__ == '__main__':
    main()

