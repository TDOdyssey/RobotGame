#ifndef _ENTITY_H
#define _ENTITY_H

#include "raylib.h"

#define MAX_ENTITIES 10

enum EntityType {
    SOCKET,
    RECT,
    SPIKE,
};

enum Flags {
    DYNAMIC         = 1 << 0,
    SOCKET_UP       = 1 << 1,
    SOCKET_DOWN     = 1 << 2,
    SOCKET_LEFT     = 1 << 3,
    SOCKET_RIGHT    = 1 << 4,
};

typedef struct Entity {
    int id;

    enum EntityType type;

    Vector2 pos;
    Vector2 vel; // simply unused in static?
    Vector2 size;

    Rectangle collision; // probably will change this in the future to be a general polygon?

    int flags;
} Entity;

Entity init_socket(Vector2 pos, int active_sockets);
Entity init_rect(Vector2 pos, Vector2 size);
Entity init_spike(Vector2 pos);

void update_entity(Entity *e);
void draw_entity(Entity e, bool selected);

void update_collision(Entity *e);

typedef struct EntitySystem {
    int     num_elements;
    int     idxs[MAX_ENTITIES];
    int     ids[MAX_ENTITIES];
    Entity  entities[MAX_ENTITIES];
} EntitySystem;

EntitySystem entity_system_init();
Entity *entity_system_add(EntitySystem *es, Entity e);
Entity *entity_system_add_with_id(EntitySystem *es, Entity e);
void    entity_system_remove_by_id(EntitySystem *es, int id);
void    entity_system_remove_by_idx(EntitySystem *es, int idx);
Entity *entity_system_get_by_id(EntitySystem *es, int id);

#endif
