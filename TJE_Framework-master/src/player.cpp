//
//  player.cpp
//  TJE_XCODE
//
//  Created by Josep Ricci on 15/5/22.
//  Copyright © 2022 Josep Ricci. All rights reserved.
//

#include "player.h"

Player* Player::instance = NULL;

Player::Player(Vector3 position, float yaw, float pitch) {
    instance = this;
    this->position = position;
    this->yaw = yaw;
    this->pitch = pitch;
}
