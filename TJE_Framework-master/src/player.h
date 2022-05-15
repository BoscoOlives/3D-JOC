//
//  player.h
//  TJE_XCODE
//
//  Created by Josep Ricci on 15/5/22.
//  Copyright © 2022 Josep Ricci. All rights reserved.
//

#ifndef player_h
#define player_h

#include "utils.h"
#include "includes.h"

class Player {
public:
    static Player* instance;
    Vector3 position;
    float yaw;
    float pitch;

    Player(Vector3 position = Vector3(), float yaw = 0.0f, float pitch = 0.0f);
};


#endif /* player_h */

