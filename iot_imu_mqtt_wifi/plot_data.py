#!/usr/bin/env python3

import matplotlib.pyplot as plt
from drawnow import *

import numpy as np
import matplotlib.animation as animation

import serial

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
  global ser, line
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
          print(lines)
          '''
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
          '''
          dat = float(lines[0])
    except:
      dat = 0
    yield t, dat

  
def main2():
  global ser, xdata, ydata, line
  #main()

  ser = serial.Serial("/dev/ttyACM0",9600)
  fig, ax = plt.subplots()
  line, = ax.plot(np.random.rand(10))
  ax.set_ylim(-10000, 10000)
  xdata, ydata = [0]*100, [0]*100

  ani = animation.FuncAnimation(fig, run, data_gen, interval=0, blit=True)
  plt.show()


if __name__ == '__main__':
  main2()

