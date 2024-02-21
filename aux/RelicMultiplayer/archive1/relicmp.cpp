#include "IConnection.h"
#include "ConnectionManager.h"
#include "ENetConnection.h"

#include "Sockets.h"

extern int  InitializeSockets();
extern void ShutdownSockets();

void ServerReceiveCallback(int connectionid, const void *data, int length, void *user)
{
    printf("Server received: '%s'\n", data);
}

void ClientReceiveCallback(int connectionid, const void *data, int length, void *user)
{
    printf("Client received: '%s'\n", data);
}

int RunClient()
{
    InitializeSockets();
    if (enet_initialize () != 0)
    {
        fprintf (stderr, "An error occurred while initializing ENet.\n");
        return -1;
    }
    atexit (enet_deinitialize);

    Connection::ENetConnection *ec = new Connection::ENetConnection;
    Connection::IConnection *connection = ec;
    bool success = ec->OpenClient();
    printf("client succeeded? %s", success ? "true" : "false");
    success = ec->Connect("127.0.0.1", 70);
    printf("client succeeded? %s", success ? "true" : "false");
    connection->SetOnReceive(ClientReceiveCallback, 0);

    while (true)
    {
        connection->Process();
        const char *message = "Hello Server!";
        connection->Send(message, strlen(message)+1, true);
    }

    connection->Close();
    delete connection;

    return 0;
}

int RunServer()
{
    InitializeSockets();
    if (enet_initialize () != 0)
    {
        fprintf (stderr, "An error occurred while initializing ENet.\n");
        return -1;
    }
    atexit (enet_deinitialize);


    Connection::ENetConnection *ec = new Connection::ENetConnection;
    Connection::IConnection *connection = ec;
    bool success = ec->OpenServer(70);
    printf("server succeeded? %s", success ? "true" : "false");
    connection->SetOnReceive(ServerReceiveCallback, 0);

    while (true)
    {
        connection->Process();
        const char *message = "Hello Client!";
//      connection->Send(message, strlen(message)+1, true);
    }

    connection->Close();
    delete connection;

    return 0;
}

int usage()
{
    printf("Need 'client' or 'server' specified!\n");
    return -1;
}

int main(int argc, const char **argv)
{
    if (argc <= 1)
        return usage();
    else if (stricmp(argv[1], "client") == 0)
        return RunClient();
    else if (stricmp(argv[1], "server") == 0)
        return RunServer();
    return usage();
}