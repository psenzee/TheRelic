#ifndef _ADDRESS_H
#define _ADDRESS_H

struct sockaddr;
struct sockaddr_in;

class Address
{
public:

    Address();
    inline Address(const Address &other) { Copy(other); }
    Address(const sockaddr_in &s);
    ~Address();

    Address &operator=(const Address &other);
    bool     operator==(const Address &other) const;

    bool        Resolve(const char *address, int port);
   
    void        SetAddress(const sockaddr_in &s);
    void        SetPort(int port);
    int         GetPort() const;
    
    const char *GetAsString() const;
    
    inline const sockaddr *GetAddress() const { return (const sockaddr *)mAddress; }

    void        Zero();
    
private:

    void Copy(const Address &other);
    
    sockaddr_in *mAddress;
};

#endif // _ADDRESS_H