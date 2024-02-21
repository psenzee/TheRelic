#ifndef _ADDRESSACQUIRER_H
#define _ADDRESSACQUIRER_H

#include "IAddress.h"

#include <vector>
#include <set>

class IAddressAcquirer
{
public:
    virtual ~IAddressAcquirer() {}

    virtual IAddress::Protocol GetProtocol() const = 0;
    virtual IAddress::Domain   GetDomain()   const = 0;

    virtual void               Register(IAddressListener &listener)   = 0;
    virtual void               Unregister(IAddressListener &listener) = 0;
    virtual void               Update()                               = 0;
};

class AbstractAddressAcquirer : public IAddressAcquirer
{
public:

    AbstractAddressAcquirer(IAddress::Domain domain, IAddress::Protocol protocol);

    IAddress::Protocol GetProtocol() const { return mProtocol; }
    IAddress::Domain   GetDomain()   const { return mDomain; }

    void               Register(IAddressListener &listener);
    void               Unregister(IAddressListener &listener);

protected:

    bool Notify(IAddress *address);

private:

    std::set<IAddressListener *> mListeners;
    IAddress::Domain             mDomain;
    IAddress::Protocol           mProtocol;
};

class AddressAcquirerManager : public IAddressAcquirer
{
public:

    AddressAcquirerManager();
    ~AddressAcquirerManager();

    IAddress::Protocol GetProtocol() const { return IAddress::PROTOCOL_ANY; }
    IAddress::Domain   GetDomain()   const { return mDomain; }

    // IAddressAcquirer implementation
    void Register(IAddressListener &listener);
    void Unregister(IAddressListener &listener);
    void Update();

private:
    
    // create and add all the acquirers
    void Initialize();
    // destroy and remove all the acquirers
    void Shutdown();

    void UpdateDomain();

    std::vector<IAddressAcquirer *> mAcquirers;
    IAddress::Domain                mDomain;
};

#endif // _ADDRESSACQUIRER_H