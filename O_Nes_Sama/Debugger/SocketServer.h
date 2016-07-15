#ifndef DEBUGGERSERVER_H
#define DEBUGGERSERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>
#include <SDL2/SDL_net.h>


class SocketServer
{
public:
    SocketServer();
    bool InitServer();
    unsigned char* ReceiveData();
    char* ReceiveString();
    void SendData(const unsigned char* Data, size_t Size);
    void SendString(const char* Str);
    void Close();
    bool InitAndWaitHandshake();
    bool PollForDebugger();
    bool connected;

private:
    UDPsocket sd;       /* Socket descriptor */
	UDPpacket *p;       /* Pointer to packet memory */
	bool initialized;

};

#endif // DEBUGGERSERVER_H
