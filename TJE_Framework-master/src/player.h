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
    Vector3 pos;
    float yaw;
    float pitch;

    Player();
};


#endif /* player_h */

