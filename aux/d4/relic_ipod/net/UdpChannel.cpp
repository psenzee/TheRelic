#include "UdpChannel.h"
#include "Sockets.h"
#include "Address.h"

#include "time/Timer.h"

UdpChannel::UdpChannel(int localPort) : 
    mSocket(0), mLocalPort(localPort)
{
    mLocal.Zero();
}

UdpChannel::~UdpChannel()
{
    Close();
}

bool UdpChannel::Open()
{
    mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (mSocket < 0)
    {
        printf("Cannot create socket, error %d\n", (int)mSocket);
        return false;
    }
    unsigned long enabled = 1;
    ioctlsocket(mSocket, FIONBIO, &enabled);
    // bind port locally
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(mLocalPort);
    mLocal.SetAddress(addr);
    int rc = bind(mSocket, (struct sockaddr *)&addr, sizeof(addr));
    if (rc < 0)
    {
        printf("Cannot bind port, error %d\n", rc);
        return false;
    }
    return true;
}

void UdpChannel::Close()
{
    closesocket(mSocket);
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

bool UdpChannel::SendTo(const Address &address, const char *data, int length)
{
    int rc = sendto(mSocket, data, length, 0, address.GetAddress(), sizeof(sockaddr_in));
    if (rc < 0)
    {
      //printf("Cannot send data (%d)!\n", errno);
        printf("Cannot send data (%s)!\n", GetSocketErrorDescription());
        return false;
    }
    ReportSentBandwidth(length);
    return true;
}

int UdpChannel::ReceiveFrom(Address &address, char *data, int length)
{
    // init buffer 
    memset(data, 0, length); // this is excessive

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    // receive message 
    int size = sizeof(sockaddr_in),
        n    = recvfrom(mSocket, 
                        data, length, 0,
                        (sockaddr *)&addr, &size);
    address.SetAddress(addr);
    if (n == -1)
    {
        //printf("Cannot receive data (%s)!\n", GetSocketErrorDescription());
        // some sort of error reporting..
        return 0;
    }
    ReportReceivedBandwidth(size);
    return n;
}