#include "SocketServer.h"

SocketServer::SocketServer() : initialized(false), connected(false)
{

}

bool SocketServer::InitServer()
{
    initialized = false;
    if (SDLNet_Init() < 0)
	{
		fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
		return false;
	}

	/* Open a socket */
	if (!(sd = SDLNet_UDP_Open(0xBA70)))
	{
		fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		return false;
	}

	/* Make space for the packet */
	if (!(p = SDLNet_AllocPacket(512)))
	{
		fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
		return false;
	}
	initialized = true;
	return true;
}

unsigned char* SocketServer::ReceiveData()
{
    if (SDLNet_UDP_Recv(sd, p))
    {
        /*printf("UDP Packet incoming\n");
        printf("\tChan:    %d\n", p->channel);
        printf("\tData:    %s\n", (char *)p->data);
        printf("\tLen:     %d\n", p->len);
        printf("\tMaxlen:  %d\n", p->maxlen);
        printf("\tStatus:  %d\n", p->status);
        printf("\tAddress: %x %x\n", p->address.host, p->address.port);*/
        p->data[511] = '\0';
        return p->data;
    }
    return NULL;
}

char* SocketServer::ReceiveString()
{
    return (char*)ReceiveData();
}

void SocketServer::SendData(const unsigned char* Data, size_t Size)
{
    //p->address.host = srvadd.host;	/* Set the destination host */
    //p->address.port = srvadd.port;	/* And destination port */
    memcpy(p->data, Data, Size);
    p->data[511] = '\0';
    p->len = strlen((char *)p->data) + 1;
    SDLNet_UDP_Send(sd, -1, p); /* This sets the p->channel */
    //printf("SEND: %s\n", Data);
}

void SocketServer::SendString(const char* Str)
{
    SendData((unsigned char*)(Str), strlen(Str)+1);
}

void SocketServer::Close()
{
    SDLNet_FreePacket(p);
	SDLNet_Quit();
	connected = false;
	initialized = false;
}

bool SocketServer::InitAndWaitHandshake()
{
    char* data;
    const char* handshake1 = "Ando buscando al debugger, aqui vive?";
    const char* handshake2 = "Si senor, aqui vive.";

    if(!InitServer())
        return false;

    while(!(data = ReceiveString()))
    {
    }
    if(strncmp(data, handshake1, 512) != 0)
    {
        //printf("%s\n", data);
        Close();
        return false;
    }

    SendString(handshake2);
    return true;
}

bool SocketServer::PollForDebugger()
{
    char* data;
    const char* handshake1 = "Ando buscando al debugger, aqui vive?";
    const char* handshake2 = "Si senor, aqui vive.";

    data = ReceiveString();
    if(data == NULL)
    {

        connected = false;
        return false;
    }

    if(strncmp(data, handshake1, 512) != 0)
    {
        Close();
        connected = false;
        return false;
    }

    SendString(handshake2);
    connected = true;
    return true;
}
