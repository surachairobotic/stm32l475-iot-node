import socket

HOST = '127.0.0.1'  # Standard loopback interface address (localhost)
PORT = 65432        # Port to listen on (non-privileged ports are > 1023)

def main():
  server = ServerTCP(HOST, PORT)
  server.start_server()
  exit()
  with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
      s.bind((HOST, PORT))
      s.listen()
      conn, addr = s.accept()
      with conn:
          print('Connected by', addr)
          while True:
              data = conn.recv(1024)
              if not data:
                  break
              conn.sendall(data)

class ServerTCP:
  def __init__(self, _host='127.0.0.1', _port=5555):
    self.host = _host
    self.port = _port
    self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
  def start_server(self):
    self.sock.bind((self.host, self.port))
    self.sock.listen(5)
    id_list = ['toa_iot','bank_iot','inn_iot']
    data = [rssiData('toa_iot'), rssiData('bank_iot'), rssiData('inn_iot')]
    i=0
    while i!=3:
      print("waiting for a client")
      conn, addr = self.sock.accept()
      print("accept")
      conn.sendall(bytes('#id$',"utf-8"))
      print("ask id")
      txt = conn.recv(64)
      txt = txt.decode('utf-8')
      for k in range(len(data)):
        if data[k].name == txt:
          print("%s : (%s, %s)" % (data[k].name, conn, addr))
          data[k].pair = (conn, addr)
      
class rssiData:
  def __init__(self, _name):
    self.name = _name
    self.pair = (0, 0)
    
if __name__ == '__main__':
  main()
