#!/bin/sh
#
/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/texturetool -m -e PVRTC --bits-per-pixel-4 -f PVR -o $1.pvrtc $1
/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/texturetool -m -e PVRTC --bits-per-pixel-4 -f PVR -o $1.pvrhi $1
/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/texturetool -m -e PVRTC --bits-per-pixel-2 -f PVR -o $1.pvrlo $1