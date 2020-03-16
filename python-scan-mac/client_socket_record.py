import socket

import asyncio
from time import sleep
from bleak import discover

from datetime import datetime

import sys

HOST = '172.20.10.3'  # Standard loopback interface address (localhost)
PORT = 3333        # Port to listen on (non-privileged ports are > 1023)
NAME = 'xxx'
#mac_addrs = ("D6:07:04:2B:F7:B1") # surachai iot
mac_addrs = ("2C:FD:A1:47:36:67") # surachai iot
b_scan = False


class ClientTCP:
  def __init__(self):
    self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
  def connect_server(self, _host='127.0.0.1', _port=5555):
    self.sock.connect((_host, _port))
    
  def terminate(self):
    self.sock.close()


client = ClientTCP()

def main():
  global client, mac_addrs, NAME, PORT

  l_name = ['toa_iot', 'bank_iot', 'inn_iot']
  NAME = l_name[int(str(sys.argv[1]))]
  print(NAME)

  HOST = str(sys.argv[2])
  PORT = int(str(sys.argv[3]))

  client.connect_server(HOST, PORT)

  loop = asyncio.get_event_loop()
  loop.create_task(scan())
  try:
      loop.run_forever()
  except SomeException as e:
      loop.close()
      print('loop.close()')
      client.terminate()
      print('terminate')
  print('END')

async def scan():
  global mac_addrs, b_scan, client, NAME
  while True:    
    txt = client.sock.recv(64)
    txt = txt.decode('utf-8')
    if txt:
      print(type(txt))
      print(txt)
    if txt == '#id$':
      print("true")
      msg = NAME
      client.sock.send(bytes(msg,"utf-8"))
    elif txt == 'save':
      b_scan = True
    elif txt == 'q':
      print('receive --> q')
      raise Exception('my exit')

    if b_scan:
        print('Start scanning')
        tstart = datetime.now()
        devices = await discover()
        print('Found %d devices'%(len(devices)))
        for i in range(len(devices)):
            print("[%d] : %s" % (i, devices[i]))
        b_found = False
        rssi = 0
        for dev in devices:
            dev_mac = str(dev).split(': ')[0]
            if dev_mac in mac_addrs:
                #print(dev_mac, 'detected at', dev.rssi, 'dBm')
                print(dev.rssi)
                rssi = dev.rssi
                b_found = True
        telapsed = datetime.now() - tstart
        print('Elapsed time: %s'%(telapsed))
        #await asyncio.sleep(6 - telapsed)
        b_scan = False
        if b_found:
          msg = str('%s:%d' % (NAME, rssi))
          client.sock.send(bytes(msg,"utf-8"))
        else:
          msg = str('%s:NOT' % (NAME))
          client.sock.send(bytes(msg,"utf-8"))

    
if __name__ == '__main__':
  main()
