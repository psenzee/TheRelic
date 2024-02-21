#include "NetGame.h"

void NetPlayerInfo::ResetAll()
{
    address = Address();
    state = NONE;
    timestamp = 0;
    pingSent = 0;
    name.Set("");
}
