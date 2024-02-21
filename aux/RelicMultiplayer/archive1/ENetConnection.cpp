#include "ENetConnection.h"

#include <stdio.h> // printf
#include <string.h> // memset

#include "enet/enet.h"
#include "time/Timer.h"

namespace Connection
{

ENetConnection::ENetConnection() 
    :  mId(0), mOpaque(0), mENetAddress(0), mLocal(0), mPeer(0), mReceiveCallback(0), mReceiveUser(0), 
       mDisconnectCallback(0), mDisconnectUser(0)
{
}

ENetConnection::~ENetConnection()
{
    Close();
}

bool ENetConnection::OpenServer(int port)
{   
    ENetHost *local = 0;
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = port;
    local = enet_host_create(&address, // $TODO specify a local address for server, if NULL, creates a client host
                             4,    // only allow 4 outgoing connection
                             0,    // unlimited downstream bandwidth,
                             0     // unlimited upstream bandwidth
                             );

    if (!local)
    {
        fprintf(stderr,  "An error occurred while trying to create an enet server host.\n");
        return false;
    }

    mLocal = local;
    /*    
    ENetEvent event;
  
    // $TODO we need to do something different here, get the address from IAddress somehow

    // Initiate the connection, allocating the two channels 0 and 1.
    ENetPeer *peer = enet_host_connect(mLocal, &address, 2);
    
    if (!peer)
    {
       fprintf(stderr, "No available peers for initiating a connection.\n");
       return false;
    }

    mPeer = peer;
    
    // Wait up to 5 seconds for the connection attempt to succeed.
    if (enet_host_service(mLocal, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
    {
        puts("Connection succeeded.");
    }
    else
    {
        // Either the 5 seconds are up or a disconnect event was 
        // received. Reset the peer in the event the 5 seconds 
        // had run out without any significant event.
        enet_peer_reset(mPeer);
        puts("Connection failed.");
        return false;
    }
    */
    return true;
}

bool ENetConnection::OpenClient()
{   
    ENetHost *local = 0;
    local = enet_host_create(NULL, // $TODO specify a local address for server, if NULL, creates a client host
                             4,    // only allow 4 outgoing connection
                             0,    // unlimited downstream bandwidth,
                             0     // unlimited upstream bandwidth
                             );

    if (!local)
    {
        fprintf(stderr,  "An error occurred while trying to create an enet client host.\n");
        return false;
    }
    
    mLocal = local;
    
    /*
    ENetAddress address;
    ENetEvent   event;
  
    // $TODO we need to do something different here, get the address from IAddress somehow
    enet_address_set_host(&address, addressStr);
    address.port = port;

    // Initiate the connection, allocating the two channels 0 and 1.
    ENetPeer *peer = enet_host_connect(mLocal, &address, 2);
    
    if (!peer)
    {
       fprintf(stderr, "No available peers for initiating a connection.\n");
       return false;
    }

    mPeer = peer;
    
    // Wait up to 5 seconds for the connection attempt to succeed.
    if (enet_host_service(mLocal, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
    {
        puts("Connection succeeded.");
    }
    else
    {
        // Either the 5 seconds are up or a disconnect event was 
        // received. Reset the peer in the event the 5 seconds 
        // had run out without any significant event.
        enet_peer_reset(mPeer);
        puts("Connection failed.");
        return false;
    }
    */
    return true;
}

bool ENetConnection::Connect(const char *addressStr, int port)
{      
    ENetAddress address;
    ENetEvent   event;
  
    // $TODO we need to do something different here, get the address from IAddress somehow
    enet_address_set_host(&address, addressStr);
    address.port = port;

    // Initiate the connection, allocating the two channels 0 and 1.
    ENetPeer *peer = enet_host_connect(mLocal, &address, 2);
    
    if (!peer)
    {
       fprintf(stderr, "No available peers for initiating a connection.\n");
       return false;
    }

    mPeer = peer;
    
    // Wait up to 5 seconds for the connection attempt to succeed.
    if (enet_host_service(mLocal, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
    {
        puts("Connection succeeded.");
    }
    else
    {
        // Either the 5 seconds are up or a disconnect event was 
        // received. Reset the peer in the event the 5 seconds 
        // had run out without any significant event.
        enet_peer_reset(mPeer);
        puts("Connection failed.");
        return false;
    }
    return true;
}

bool ENetConnection::Close()
{
    ENetEvent event;
    memset(&event, 0, sizeof(ENetEvent));
    
    enet_peer_disconnect(mPeer, 0);

    // Allow up to 3 seconds for the disconnect to succeed and drop any packets received packets. 
    while (enet_host_service(mLocal, &event, 3000) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_RECEIVE:
            enet_packet_destroy(event.packet);
            break;

        case ENET_EVENT_TYPE_DISCONNECT:
            puts("Disconnection succeeded.");
            return true;
        }
    }
    
    // We've arrived here, so the disconnect attempt didn't */
    // succeed yet.  Force the connection down.             */
    enet_peer_reset(mPeer);
    enet_host_destroy(mLocal);

    return true;
}

enum { TOTAL_TIME = 5 };

static void ReportSentBandwidth(int bytes)
{
    static unsigned total = 0, lastTime = GetCurrentTimeMs();
    total += bytes;
    if (GetCurrentTimeMs() > lastTime + TOTAL_TIME * 1000)
    {
        lastTime = GetCurrentTimeMs();
        printf("Data rate (bytes sent) last %d seconds = %.2fkbps\n", TOTAL_TIME, total * 8 / static_cast<float>(TOTAL_TIME * 1024));
        total = 0;
    }
}

static void ReportReceivedBandwidth(int bytes)
{
    static unsigned total = 0, lastTime = GetCurrentTimeMs();
    total += bytes;
    if (GetCurrentTimeMs() > lastTime + TOTAL_TIME * 1000)
    {
        lastTime = GetCurrentTimeMs();
        printf("Data rate (bytes received) last %d seconds = %.2fkbps\n", TOTAL_TIME, total * 8 / static_cast<float>(TOTAL_TIME * 1024));
        total = 0;
    }
}

bool ENetConnection::Send(const char *data, int length, bool reliable, bool sequenced)
{
    // Create a reliable packet containing data
    int options = (reliable ? ENET_PACKET_FLAG_RELIABLE : 0) | (sequenced ? 0 : ENET_PACKET_FLAG_UNSEQUENCED);
    ENetPacket *packet = enet_packet_create(data, length, options);
    
    // Send the packet to the peer over channel id 0.
    // One could also broadcast the packet by
    // enet_host_broadcast (host, 0, packet);
    enet_peer_send(mPeer, 0, packet);

    // One could just use enet_host_service() instead. 
    enet_host_flush(mLocal);
    
    ReportSentBandwidth(length);
    return true;
}

void ENetConnection::Process()
{
    ENetEvent event;
    memset(&event, 0, sizeof(ENetEvent));    
    
    // Wait up to 1000 milliseconds for an event.
    if (enet_host_service(mLocal, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            printf("A new client connected from %x:%u.\n", 
                   event.peer->address.host,
                   event.peer->address.port);
            // $TODO Store any relevant client information here. 
            event.peer->data = (void*)"Client information";
            break;

        case ENET_EVENT_TYPE_RECEIVE:
            printf("A packet of length %u containing %s was received from %s on channel %u.\n",
                   event.packet->dataLength,
                   event.packet->data,
                   event.peer->data,
                   event.channelID);
            ReportReceivedBandwidth(event.packet->dataLength);                   
            if (mReceiveCallback)
                mReceiveCallback(mId, event.packet->data, event.packet->dataLength, mReceiveUser);
            // Clean up the packet now that we're done using it.
            enet_packet_destroy(event.packet);
            break;
           
        case ENET_EVENT_TYPE_DISCONNECT:
            printf("%s disconected.\n", event.peer->data);
            // Reset the peer's client information.
            event.peer->data = NULL;
            break;
        }
    }
}

}