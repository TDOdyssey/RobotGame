#include "raylib.h"

#include "entity.h"

#include "gui.h"

#include <math.h>
#include <stdio.h>

#define BLOCK_SIZE 40.f

Entity init_socket(Vector2 pos, int flags)
{
    Entity e;
    e.type = SOCKET;
    e.pos = pos;
    e.size = (Vector2){ BLOCK_SIZE, BLOCK_SIZE };
    e.collision = (Rectangle){
        e.pos.x - e.size.x/2,
        e.pos.y - e.size.y/2,
        e.size.x,
        e.size.y,
    };
    e.flags = flags;
    return e;
}

void update_collision(Entity *e)
{
    e->collision = (Rectangle){
        e->pos.x - e->size.x/2,
        e->pos.y - e->size.y/2,
        e->size.x,
        e->size.y,
    };
}

Entity init_rect(Vector2 pos, Vector2 size)
{
    Entity e;
    e.type = RECT;
    e.pos = pos;
    e.size = size;
    e.collision = (Rectangle){
        e.pos.x - e.size.x/2,
        e.pos.y - e.size.y/2,
        e.size.x,
        e.size.y,
    };
    e.flags = 0;
    return e;
}

Entity init_spike(Vector2 pos)
{
    return (Entity){0};
}

void update_entity(Entity *e)
{
    if(!(e->flags & DYNAMIC))
        return;
    
    Vector2 accel = {0.f, 0.f};
    switch(e->type)
    {
        default:
            break;
    }
}

void draw_entity(Entity e, bool selected)
{
    switch(e.type) {
        case SOCKET:
            DrawRectangle(e.pos.x - e.size.x/2.f, e.pos.y - e.size.y/2.f, e.size.x, e.size.y, DARKGRAY);
            if(e.flags & SOCKET_UP)
                DrawCircle(e.pos.x, e.pos.y - e.size.y/2.f, 5.f, BLACK);
            if(e.flags & SOCKET_DOWN)
                DrawCircle(e.pos.x, e.pos.y + e.size.y/2.f, 5.f, BLACK);
            if(e.flags & SOCKET_LEFT)
                DrawCircle(e.pos.x - e.size.x/2.f, e.pos.y, 5.f, BLACK);
            if(e.flags & SOCKET_RIGHT)
                DrawCircle(e.pos.x + e.size.x/2.f, e.pos.y, 5.f, BLACK);

            if(selected)
                DrawRectangleLinesEx(e.collision, 2.f, get_selection_color());
            break;
        case RECT:
            DrawRectangleRec(e.collision, DARKBLUE);
            if(selected)
                DrawRectangleLinesEx(e.collision, 2.f, get_selection_color());
        case SPIKE:
            break;
        default:
            break;
    }
}

EntitySystem entity_system_init()
{
    EntitySystem es;
    es.num_elements = 0;
    for(int i = 0; i < MAX_ENTITIES; i++)
    {
        es.ids[i] = i;
        es.idxs[i] = i;
    }
    
    return es;
}

Entity *entity_system_add(EntitySystem *es, Entity e)
{
    if(es->num_elements == MAX_ENTITIES)
        return NULL;

    int id = es->ids[es->num_elements];
    e.id = id;
    es->idxs[id] = es->num_elements;
    es->entities[es->num_elements] = e;

    return &es->entities[es->num_elements++];
}

Entity *entity_system_add_with_id(EntitySystem *es, Entity e)
{
    if(es->num_elements == MAX_ENTITIES)
        return NULL;

    int id = e.id;
    if(id >= MAX_ENTITIES)
        return NULL;
    
    if(es->idxs[id] < es->num_elements)
    {
        printf("ERROR: There already exists an entity with this id: %d!\n", id);
        return NULL;
    }

    int idx = es->idxs[id];
    es->ids[idx] = es->ids[es->num_elements];
    es->ids[es->num_elements] = id;

    es->idxs[id] = es->num_elements;
    es->idxs[es->ids[idx]] = idx;

    es->entities[es->num_elements] = e;

    return &es->entities[es->num_elements++];
}

void entity_system_remove_by_id(EntitySystem *es, int id)
{
    es->num_elements--;

    int idx = es->idxs[id];
    es->entities[idx] = es->entities[es->num_elements];
    es->ids[idx] = es->ids[es->num_elements];
    es->ids[es->num_elements] = id;

    es->idxs[id] = es->num_elements;
    es->idxs[es->ids[idx]] = idx;
}

void entity_system_remove_by_idx(EntitySystem *es, int idx)
{
    es->num_elements--;

    int id = es->ids[idx];

    es->entities[idx] = es->entities[es->num_elements];
    es->ids[idx] = es->ids[es->num_elements];
    es->ids[es->num_elements] = id;

    es->idxs[id] = es->num_elements;
    es->idxs[es->ids[idx]] = idx;
}

Entity *entity_system_get_by_id(EntitySystem *es, int id)
{
    return &es->entities[es->idxs[id]];
}
