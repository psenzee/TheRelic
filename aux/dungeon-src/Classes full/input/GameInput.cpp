/*
 *  GameInput.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/29/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "GameInput.h"

#include <stdio.h>

void GameInput::GetInput()
{
    if (touched)
    {
        printf("Screen touched at (%u, %u)!\n", touch.x, touch.y);
//      resume.SetState(true);
        start.SetState(true);
        touched = false;
    }
}