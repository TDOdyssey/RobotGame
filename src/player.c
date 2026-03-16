#include "player.h"

Player init_player(Vector2 pos, int attached_id, enum AttachedDir dir)
{
    Player p;
    p.pos = pos;
    p.vel = (Vector2){ 0.f, 0.f };
    p.power = 1.0f;
    p.attached_id = attached_id;
    p.attached_dir = dir;
    p.can_attach_timer = 0.f;
    return p;
}

void draw_player(Player p)
{
    DrawCircleV(p.pos, PLAYER_RADIUS, p.power <= 0 ? DARKGRAY : GRAY);
    DrawRing(p.pos, PLAYER_RADIUS + 5.f, PLAYER_RADIUS + 10.f, 0.f, 360.f * p.power, 0, GREEN);
}