import socket

HOST = '127.0.0.1'  # Standard loopback interface address (localhost)
PORT = 3333        # Port to listen on (non-privileged ports are > 1023)

def main():
  server = ServerTCP(HOST, PORT)
  people = server.start_server()
  
  f = open('/home/cobot/taist/software_embedded_systems/stm32l475-iot-node/python-scan-mac/sync_data.txt', 'w+')
  f.close()
  
  key = 'x'
  while True:
    key = input('Enter your input: ')
    if key == 's':
      for i in range(len(people)):
        people[i].conn.sendall(bytes('save',"utf-8"))
    elif key == 'q':
      break
    incoming_txt = ["", "", ""]
    for i in range(len(people)):
      incoming_txt[i] = people[i].conn.recv(1024).decode('utf-8')
    print(incoming_txt)
    f = open('/home/cobot/taist/software_embedded_systems/stm32l475-iot-node/python-scan-mac/sync_data.txt', 'a')
    f.writelines(str("\n%s,%s,%s" % (incoming_txt[0], incoming_txt[1], incoming_txt[2])))
    f.close()

  for i in range(len(people)):
    print('send q : ', i)
    people[i].conn.sendall(bytes('q',"utf-8"))

  exit()

class ServerTCP:
  def __init__(self, _host='127.0.0.1', _port=5555):
    self.host = _host
    self.port = _port
    self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
  def start_server(self):
    self.sock.bind((self.host, self.port))
    self.sock.listen(5)
    id_list = ['toa_iot','bank_iot','inn_iot']
    people = [rssiPeople('toa_iot'), rssiPeople('bank_iot'), rssiPeople('inn_iot')]
    i=0
    while i!=3:
      print("waiting for a client")
      conn, addr = self.sock.accept()
      print("accept")
      conn.sendall(bytes('#id$',"utf-8"))
      print("ask id")
      txt = conn.recv(1024)
      txt = txt.decode('utf-8')
      print(txt)
      for k in range(len(people)):
        if people[k].name == txt:
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
