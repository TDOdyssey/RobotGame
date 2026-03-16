#ifndef _PLAYER_H
#define _PLAYER_H

#include "raylib.h"

#define PLAYER_RADIUS 20.f
#define PLAYER_SPEED 800.f
#define PLAYER_BOUNCE_DAMP 0.5f
#define PLAYER_JUMP_VEL 400.f
#define PLAYER_LAUNCH_VEL 600.f

enum AttachedDir {
    ATTACHED_UP = 0,
    ATTACHED_DOWN = 1,
    ATTACHED_LEFT = 2,
    ATTACHED_RIGHT = 3,
};

typedef struct Player {
    Vector2 pos;
    Vector2 vel;
    float power;
    int attached_id;
    enum AttachedDir attached_dir;
    float can_attach_timer;
} Player;

Player init_player(Vector2 pos, int attached_id, enum AttachedDir dir);
void draw_player(Player p);

#endif