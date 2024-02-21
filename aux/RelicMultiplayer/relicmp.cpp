#include "IConnection.h"
#include "ConnectionManager.h"
#include "ENetConnection.h"

#include "Sockets.h"

extern int  InitializeSockets();
extern void ShutdownSockets();

void _ShutdownENet()
{
    enet_deinitialize();
    ShutdownSockets();
}

bool StartENet()
{
    InitializeSockets();
    if (enet_initialize () != 0)
    {
        fprintf(stderr, "An error occurred while initializing ENet.\n");
        return false;
    }
    atexit(_ShutdownENet);
    return true;
}

Connection::ConnectionManager *CreateENetClientConnectionManager(const char *address, int port)
{
    Connection::ConnectionManager *cm = new Connection::ConnectionManager;
    Connection::ENetClientConnector *cc = new Connection::ENetClientConnector;
    cc->SetAddress(address, port);
    cm->SetClientConnector(cc);
    return cm;
}

Connection::ConnectionManager *CreateENetServerConnectionManager(int port)
{
    Connection::ConnectionManager *cm = new Connection::ConnectionManager;
    Connection::ENetServerConnector *sc = new Connection::ENetServerConnector;
    sc->Open(port);
    cm->SetServerConnector(sc);
    return cm;
}

void ServerReceiveCallback(int connectionid, const void *data, int length, void *user)
{
    Connection::ConnectionManager *cm = static_cast<Connection::ConnectionManager *>(user);
    printf("Server received: '%s', responding\n", data);
    char message[128];
    sprintf(message, "Hi '%s'!", cm->GetName(connectionid));
    cm->Send(connectionid, message, sizeof(message) + 1, false, false);
}

void ClientReceiveCallback(int connectionid, const void *data, int length, void *user)
{
    printf("Client received: '%s'\n", data);
}

void OnConnectServer(Connection::IConnection *connection, void *user)
{
    printf("Server (me) now connected to client %d!\n", connection->GetId());
    printf("New connection %p\n", connection);
    /*
    const char *message = "Hello Client!";
    connection->Send(message, strlen(message) + 1, true);
    */
}

void OnConnectClient(Connection::IConnection *connection, void *user)
{
    printf("Client (me) now connected to server!\n");
    printf("New connection %p\n", connection);
    /*
    const char *message = "Hello Server!";
    connection->Send(message, strlen(message) + 1, false, false);
    */
}

void OnDisconnect(int connectionid, int reason, void *user)
{
    printf("Remote %d disconnected!\n", connectionid);
}

int RunClient(int client)
{
    printf("Client\n------\n");
    StartENet();
    Connection::ConnectionManager *cm = CreateENetClientConnectionManager("127.0.0.1", 70);
    cm->SetOnConnect(OnConnectClient, 0);
    cm->SetOnDisconnect(OnDisconnect, 0);
    cm->SetOnReceive(ClientReceiveCallback, 0);
    char name[128];
    sprintf(name, "Client %d", client);
    cm->Start(name);
static int count = 0;
    while (true)
    {
        cm->Process();
     //   cm->SendAllString("Hi S");
        ::Sleep(8);
if (count % 200 == 0)
{
cm->SendAllString("Hi S", false, false);
cm->PrintConnections();
}
count++;
    }
    delete cm;
    return 0;
}

int RunServer()
{
    printf("Server\n------\n");
    StartENet();
    Connection::ConnectionManager *cm = CreateENetServerConnectionManager(70);
    cm->SetOnConnect(OnConnectServer, 0);
    cm->SetOnDisconnect(OnDisconnect, 0);
    cm->SetOnReceive(ServerReceiveCallback, cm);
    cm->Start("Server");
static int count = 0;
    while (true)
    {
        cm->Process();
        ::Sleep(8);
if (count % 100 == 0)
{
cm->PrintConnections();
}
count++;
    }
    delete cm;
    return 0;
}

int usage()
{
    printf("Need 'client1', 'client2', 'client3', 'client4' or 'server' specified!\n");
    return -1;
}

int main(int argc, const char **argv)
{
    if (argc <= 1)
        return usage();
    else if (stricmp(argv[1], "client1") == 0)
        return RunClient(1);
    else if (stricmp(argv[1], "client2") == 0)
        return RunClient(2);
    else if (stricmp(argv[1], "client3") == 0)
        return RunClient(3);
    else if (stricmp(argv[1], "client4") == 0)
        return RunClient(4);
    else if (stricmp(argv[1], "server") == 0)
        return RunServer();
    return usage();
}