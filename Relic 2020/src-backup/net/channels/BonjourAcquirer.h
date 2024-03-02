#ifndef _BONJOURACQUIRER_H
#define _BONJOURACQUIRER_H

#include "AddressAcquirer.h"

class BonjourAcquirer : public AbstractAddressAcquirer
{
public:
    BonjourAcquirer();

    void Update();
};

#endif // _BONJOURACQUIRER_H