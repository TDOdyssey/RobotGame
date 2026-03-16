#ifndef _LEVEL_H
#define _LEVEL_H

#include "entity.h"
#include "player.h"

#define MAX_ENTITIES 10

typedef struct Level {
    float w, h;

    char *filename;

    Player initial_player;
    Player p;

    EntitySystem entity_system_initial;
    EntitySystem entity_system;
} Level;

bool save_level(Level *level, const char *filename);
bool load_level(Level *level, const char *filename);
void reset_level(Level *lvl);
void update_player(Level lvl, Player *e);

void put_player_in_attached_position(EntitySystem *es, Player *p);

#endif
