import socket
import sys

HOST = '0.0.0.0'  # Standard loopback interface address (localhost)
PORT = 3333        # Port to listen on (non-privileged ports are > 1023)
fname = '/home/cobot/taist/software_embedded_systems/stm32l475-iot-node/python-scan-mac/sync_data_in.txt'


def main():
  global fname, PORT

  PORT = int(str(sys.argv[1]))

  server = ServerTCP(HOST, PORT)
  people = server.start_server()
  
  f = open(fname, 'a+')
  f.writelines('start...')
  f.close()
  
  key = 'x'
  while True:
    key = input('Enter your input: ')
    if key == 's':
      _iter = 1
    elif key == 'a':
      _iter = 5
    elif key == 'q':
      break
    j=0
    while j<_iter:
      #for i in range(len(people)):
      people[0].conn.sendall(bytes('save',"utf-8"))
      incoming_txt = ["", "", ""]
      #for i in range(len(people)):
      tmp_txt = people[0].conn.recv(1024)
      print(tmp_txt, " : type-> ", type(tmp_txt))
      incoming_txt[0] = tmp_txt.decode('utf-8')
      #incoming_txt[0] = people[0].conn.recv(1024).decode('utf-8')
      print(incoming_txt)
      b_complete = True
      for i in range(len(incoming_txt)):
        if incoming_txt[i].find('NOT') != -1:
          b_complete = False
          break
      if b_complete:
        print(incoming_txt)
        f = open(fname, 'a')
        f.writelines(str("%s,%s,%s\r\n" % (incoming_txt[0], incoming_txt[1], incoming_txt[2])))
        f.close()
        j=j+1

  #for i in range(len(people)):
  print('send q : ', i)
  people[0].conn.sendall(bytes('q',"utf-8"))

  f = open(fname, 'a')
  f.writelines('end...')
  f.close()


class ServerTCP:
  def __init__(self, _host='127.0.0.1', _port=5555):
    self.host = _host
    self.port = _port
    self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
  def start_server(self):
    self.sock.bind((self.host, self.port))
    self.sock.listen(10)
    id_list = ['toa_iot','bank_iot','inn_iot']
    people = [rssiPeople('toa_iot'), rssiPeople('bank_iot'), rssiPeople('inn_iot')]
    i=0
    while i!=1:
      print("waiting for a client")
      conn, addr = self.sock.accept()
      print("accept")
      conn.sendall(bytes('#id$',"utf-8"))
      print("ask id")
      txt = conn.recv(1024)
      txt = txt.decode('utf-8')
      print(txt)
      for k in range(len(people)):
        #print(people[k].name, " : ", type(people[k].name), " : len= ", len(people[k].name))
        #print(txt, " : ", type(txt), " : len= ", len(txt))
        #print(people[k].name, " : ", txt, " : find=", people[k].name.find(txt))
        #for t in people[k].name:
        #  print(t, " : ", int(t))
        #for t in txt:
        #  print(t, " : ", int(t))
        if people[k].name.find(txt) != -1:
          print("%s : (%s, %s)" % (people[k].name, conn, addr))
          print(type(conn))
          print(type(addr))
          people[k].conn = conn
          i+=1
    return people
      
class rssiPeople:
  def __init__(self, _name):
    self.name = _name
    self.conn = socket.socket()
    
if __name__ == '__main__':
  main()
