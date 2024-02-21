#include "IAddress.h"
#include "LobbyAcquirer.h"

LobbyAcquirer::LobbyAcquirer() 
  : AbstractAddressAcquirer(IAddress::DOMAIN_INTERNET, IAddress::PROTOCOL_TCP)
{
} 

void LobbyAcquirer::Update()
{
    // $TODO
}
