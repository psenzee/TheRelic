/*
 *  Player.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/30/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "Player.h"
#include "GameState.h"

#include "core/xna.h"

#include "objects/RollingBall.h"
#include "input/GameInput.h"
#include "map/Map.h"

Player::Player(GameState *state, XmlElement *level, const Vector2 &position) : BallCharacter(state, level, position), IsAlive(true)
{
    LoadContent("Ball");
}

void Player::Reset(const Vector2 &position)
{
    BallCharacter::Reset(position);
    IsAlive = true;
}

void Player::OnKilled()
{
    IsAlive = false;
}
