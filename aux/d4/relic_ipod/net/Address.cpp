#include "Address.h"
#include "Sockets.h"

bool Address::Resolve(const char *address, int port)
{
    memset(mAddress, 0, sizeof(sockaddr_in));
    hostent *host = gethostbyname(address);
    if (!host)
    {
        printf("Unknown host '%s'\n", address);
        return false;
    }
    
    printf("Resolving '%s' to '%s' (IP : %s) \n",
           address,
           host->h_name,
           inet_ntoa(*(struct in_addr *)host->h_addr_list[0]));
    
    //mAddress->sin_family = host->h_addrtype;
    mAddress->sin_family = AF_INET;
    memcpy(&(mAddress->sin_addr.s_addr), host->h_addr_list[0], host->h_length);
    mAddress->sin_port = htons(port);
    return true;
}

void Address::SetPort(int port)
{
    mAddress->sin_port = htons(port);
}

const char *Address::GetAsString() const
{
    static char address[1024];
    // print received message 
    sprintf(address, "%s:%u",
            inet_ntoa(mAddress->sin_addr),
            ntohs(mAddress->sin_port));
    return address;
}

Address::Address() : mAddress(0)
{
    mAddress = new sockaddr_in;
    memset(mAddress, 0, sizeof(sockaddr_in));
    mAddress->sin_family = AF_INET;
}

Address::Address(const sockaddr_in &s) : mAddress(0)
{
    mAddress = new sockaddr_in;
    memcpy(mAddress, &s, sizeof(sockaddr_in));
}

void Address::Zero()
{
    memset(mAddress, 0, sizeof(sockaddr_in));
}

void Address::Copy(const Address &other)
{
    if (mAddress)
        delete mAddress;
    mAddress = new sockaddr_in;
    memcpy(mAddress, other.mAddress, sizeof(sockaddr_in));    
}

Address::~Address()
{
    delete mAddress;
    mAddress = 0;
}

Address &Address::operator=(const Address &other)
{
    if (&other != this)
        Copy(other);
    return *this;
}

bool Address::operator==(const Address &other) const
{
    return other.mAddress != 0 && mAddress != 0 && memcmp(other.mAddress, mAddress, sizeof(sockaddr_in)) == 0;
}

void Address::SetAddress(const sockaddr_in &s)
{ 
    memcpy(mAddress, &s, sizeof(sockaddr_in));
}

/* // NOT NECESSARY NOW

char *cpp_strdup(const char *s)
{
    if (!s) return 0;
    size_t len = strlen(s);
    char *c = new char [len + 1];
    memset(c, 0, len + 1);
    strnzcpy(c, s, len);
    return c;
}

int cpp_strlistcount(const char **p)
{
    if (!p || !*p) return 0;
    int count = 0;
    for (const char **plist = p; *plist; count++, plist++) {} // count items..
    return count;
}

char **cpp_strlistdup(const char **p)
{
    int count = cpp_strlistcount(p);
    char **nlist = new char * [count + 1];
    memset(nlist, 0, sizeof(char *) * (count + 1));
    for (int i = 0; i < count; i++)
      nlist[i] = cpp_strdup(p[i]);
    return nlist;
}

void destroy_strlist(char **p)
{
    if (p)
    {
        for (char **plist = p; *plist; plist++)
            delete [] *plist;
        delete [] p;
    }
}

void copy(const hostent *src, hostent *dst)
{
    memcpy(dst, src, sizeof(hostent));
    dst.h_name      = cpp_strdup(dst.h_name);
    dst.h_aliases   = cpp_strlistdup(dst.h_aliases);
    dst.h_addr_list = cpp_strlistdup(dst.h_addr_list);
}

void destroy(hostent *h)
{
    if (dst.h_name) delete [] dst.h_name;
    destroy_strlist(dst.h_aliases);
    destroy_strlist(dst.h_addr_list);
}

*/