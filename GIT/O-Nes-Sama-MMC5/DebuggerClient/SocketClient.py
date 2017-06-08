# Echo client program
import socket


class SocketClient:
    def __init__(self):
        self.__socket = None
        self.__addr = None
        self._connected = False

    def ConnectToServer(self, Ip, Port):
        self.__socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.__addr = (Ip, Port)
        #self.Send('Ando buscando al debugger, aqui vive?')
        self.__socket.sendto('Ando buscando al debugger, aqui vive?\0', self.__addr)
        string = self.__socket.recv(512)
        if string != "Si senor, aqui vive.\0":
            print "NO DEBUGGER!"
            print repr(string)
            return False
        self._connected = True
        return True

    def Send(self, Data):
        if self._connected:
            self.__socket.sendto(Data+'\0', self.__addr)
            return True
        else:
            return False

    def Receive(self):
        return self.__socket.recv(512).rstrip('\0')

    def CloseSocket(self):
        self.__socket.close()
        self._connected = False

    def SetBlocking(self, blocking):
        self.__socket.setblocking(blocking)

if __name__ == '__main__':
    HOST = 'localhost'  # The remote host
    PORT = 0xBA70  # The same port as used by the server

    dg = SocketClient()
    dg.ConnectToServer(HOST, PORT)

    for line in iter(lambda: raw_input(">>>"), ""):
        dg.Send(line + "\0")
        data = dg.Receive()
        if line == "QUIT":
            break
        print 'Received:', repr(data)
    dg.CloseSocket()
