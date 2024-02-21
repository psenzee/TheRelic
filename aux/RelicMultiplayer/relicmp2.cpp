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

int RunClient(int client)
{
    InitializeSockets();
    if (enet_initialize () != 0)
    {
        fprintf (stderr, "An error occurred while initializing ENet.\n");
        return -1;
    }
    atexit (enet_deinitialize);

    Connection::ENetClientConnection *ec = new Connection::ENetClientConnection;
    Connection::IConnection *connection = ec;
    bool success = ec->Connect("127.0.0.1", 70);
    printf("client succeeded? %s\n", success ? "true" : "false");
    connection->SetOnReceive(ClientReceiveCallback, 0);

    while (true)
    {
        connection->Process();
        char message[1024];
        sprintf(message, "Hello Server from %d!", client);
        connection->Send(message, strlen(message)+1, true);
       ::Sleep(33);
    }

    connection->Close();
    delete connection;

    return 0;
}

std::vector<Connection::IConnection *> connections;

void OnConnect(Connection::IConnection *connection, void *user)
{
    printf("New connection %p\n", connection);
    const char *message = "Hello Client!";
    connection->SetOnReceive(ServerReceiveCallback, 0);
    connection->Send(message, strlen(message)+1, true);
    connections.push_back(connection);
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

    Connection::ENetServerConnector *esc = new Connection::ENetServerConnector;
    esc->SetOnConnect(OnConnect, 0);
    esc->Open(70);
    while (true)
    {
        esc->Process();
        for (std::vector<Connection::IConnection *>::iterator i = connections.begin(), e = connections.end(); i != e; ++i)
        {
            (*i)->Send("Hi", 3, true);
            (*i)->Process();
            ::Sleep(33);
        }
    }

    delete esc;

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