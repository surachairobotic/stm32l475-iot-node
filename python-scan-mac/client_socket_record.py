import socket

HOST = '127.0.0.1'  # Standard loopback interface address (localhost)
PORT = 65432        # Port to listen on (non-privileged ports are > 1023)
NAME = 'toa_iot'

def main():
  client = ClientTCP()
  client.connect_server(HOST, PORT)
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

class ClientTCP:
  def __init__(self):
    self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
  def connect_server(self, _host='127.0.0.1', _port=5555):
    self.sock.connect((_host, _port))
    
if __name__ == '__main__':
  main()
