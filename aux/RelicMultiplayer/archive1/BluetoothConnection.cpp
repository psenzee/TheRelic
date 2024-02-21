#include "BluetoothConnection.h"

#include <stdio.h> // printf
#include <string.h> // memset

#include "time/Timer.h"

BluetoothConnection::BluetoothConnection(/*const IAddress *address*/) 
    :  /*mAddress(address), */mReceiveCallback(0), mReceiveUser(0), mDisconnectCallback(0), mDisconnectUser(0)
{
}

BluetoothConnection::~BluetoothConnection()
{
    Close();
}

bool BluetoothConnection::Open()
{   
    return false;
}

void BluetoothConnection::Close()
{
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

bool BluetoothConnection::Send(const char *data, int length, bool reliable, bool sequenced)
{
    // $TODO ..    
    ReportSentBandwidth(length);
    return false;
}

void BluetoothConnection::Process()
{
    // $TODO
    /*   
    ReportReceivedBandwidth(event.packet->dataLength);                   
    if (mReceiveCallback)
        mReceiveCallback(event.packet->data, event.packet->dataLength, mReceiveUser);    
    */

}
