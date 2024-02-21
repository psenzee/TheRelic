#ifndef _BLUETOOTHACQUIRER_H
#define _BLUETOOTHACQUIRER_H

#include "AddressAcquirer.h"

class BluetoothAcquirer : public AbstractAddressAcquirer
{
public:
    BluetoothAcquirer();

    void Update();
};

#endif // _BLUETOOTHACQUIRER_H