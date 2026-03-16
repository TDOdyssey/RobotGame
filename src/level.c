#include "level.h"

#include <stdio.h>
#include <string.h>

#include "raymath.h"

bool save_level(Level *level, const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if(!fp)
    {
        printf("ERROR: Unable to load file \"%s\"\n", filename);
        return false;
    }

    fprintf(fp, "level %d %d\n", (int)level->w, (int)level->h);
    fprintf(fp, "player %d %d\n", level->initial_player.attached_id, (int)level->initial_player.attached_dir);
    for(int i = 0; i < level->entity_system_initial.num_elements; i++)
    {
        Entity e = level->entity_system_initial.entities[i];
        fprintf(fp, "entity %d %d %d %d %d %d %d\n", e.id, (int)e.type, (int)e.pos.x, (int)e.pos.y, (int)e.size.x, (int)e.size.y, e.flags);
    }

    fclose(fp);
    return true;
}

bool load_level(Level *level, const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if(!fp)
    {
        printf("ERROR: Unable to load file \"%s\"\n", filename);
        return false;
    }

    level->entity_system_initial = entity_system_init();
    level->entity_system = entity_system_init();

    level->w = 800.f;
    level->h = 600.f;

    size_t len = 0;
    char buf[128];
    while((len = fscanf(fp, "%s", buf)) != 0 && (len != EOF))
    {
        if(strcmp(buf, "level") == 0) {
            float w, h;
            int r = fscanf(fp, "%f %f", &w, &h);
            if(r == 0 || r == EOF)
            {
                printf("ERROR: Couldn't parse level size!\n");
                return false;
            }

            level->w = w;
            level->h = h;
        }
        else if(strcmp(buf, "player") == 0)
        {
            int idx, dir;
            int r = fscanf(fp, "%d %d", &idx, &dir);
            if(r == 0 || r == EOF)
            {
                printf("ERROR: Couldn't find player in level file!\n");
                return false;
            }

            level->initial_player = init_player((Vector2){0, 0}, idx, dir);
        }/*
        else if(strcmp(buf, "socket") == 0)
        {
            float x, y;
            int points;
            int r = fscanf(fp, "%f %f %d", &x, &y, &points);
            if(r == 0 || r == EOF)
            {
                printf("Warning: Failed to load at least one entity!\n");
                continue;
            }

            entity_system_add(&level->entity_system_initial, init_socket((Vector2){x, y}, points));
        }
        else if(strcmp(buf, "rect") == 0)
        {
            float x, y, w, h;
            int r = fscanf(fp, "%f %f %f %f", &x, &y, &w, &h);
            if(r == 0 || r == EOF)
            {
                printf("Warning: Failed to load at least one entity!\n");
                continue;
            }

            entity_system_add(&level->entity_system_initial, init_rect((Vector2){x, y}, (Vector2){w, h}));
        }*/
        else if(strcmp(buf, "entity") == 0)
        {
            Entity e = {0};
            int r = fscanf(fp, "%d %d %f %f %f %f %d", &e.id, (int*)&e.type, &e.pos.x, &e.pos.y, &e.size.x, &e.size.y, &e.flags);
            if(r != 7)
            {
                printf("Warning: Unable to process entity\n");
                continue;
            }
            update_collision(&e);
            entity_system_add_with_id(&level->entity_system_initial, e);
        }
    }

    fclose(fp);

    level->filename = filename;

    put_player_in_attached_position(&level->entity_system_initial, &level->initial_player);

    reset_level(level);

    return true;
}

void reset_level(Level *lvl)
{
    lvl->p = lvl->initial_player;
    memcpy(&lvl->entity_system, &lvl->entity_system_initial, sizeof(EntitySystem));
}

void update_player(Level lvl, Player *p)
{
    for(int i = 0; i < lvl.entity_system.num_elements; i++)
    {
        Entity *e = &lvl.entity_system.entities[i];
        if(e->type == SOCKET && p->can_attach_timer <= 0.f && p->attached_id == -1)
        {
            Rectangle u = e->collision;
            Rectangle d = e->collision;
            Rectangle l = e->collision;
            Rectangle r = e->collision;

            u.width     = PLAYER_RADIUS*2;
            u.height    = PLAYER_RADIUS*2;
            d.width     = PLAYER_RADIUS*2;
            d.height    = PLAYER_RADIUS*2;
            l.width     = PLAYER_RADIUS*2;
            l.height    = PLAYER_RADIUS*2;
            r.width     = PLAYER_RADIUS*2;
            r.height    = PLAYER_RADIUS*2;

            u.y -= PLAYER_RADIUS*2;
            d.y += e->size.y;
            l.x -= PLAYER_RADIUS*2;
            l.y += e->size.y / 2;
            r.x += e->size.x;
            r.y += e->size.y / 2;

            p->attached_id = e->id;
            if(e->flags & SOCKET_UP && CheckCollisionCircleRec(p->pos, PLAYER_RADIUS, u)) {
                p->attached_dir = ATTACHED_UP;
            }
            else if(e->flags & SOCKET_DOWN && CheckCollisionCircleRec(p->pos, PLAYER_RADIUS, d)) {
                p->attached_dir = ATTACHED_DOWN;
            }
            else if(e->flags & SOCKET_LEFT && CheckCollisionCircleRec(p->pos, PLAYER_RADIUS, l)) {
                p->attached_dir = ATTACHED_LEFT;
            }
            else if(e->flags & SOCKET_RIGHT && CheckCollisionCircleRec(p->pos, PLAYER_RADIUS, r)) {
                p->attached_dir = ATTACHED_RIGHT;
            }
            else {
                p->attached_id = -1;
            }
        }

    }

    put_player_in_attached_position(&lvl.entity_system, p);

    Vector2 accel = {0.f, 0.f};
    if(p->attached_id >= 0)
    {
        if(p->power >= 1.0f)
            p->power = 1.0f;
        else
            p->power += GetFrameTime();

        if(IsKeyReleased(KEY_SPACE))
        {
            p->vel = (Vector2){0.f, 0.f};
            p->can_attach_timer = 0.1f;
            p->attached_id = -1;
            if(p->attached_dir == ATTACHED_UP)
                p->vel = (Vector2){0, -PLAYER_LAUNCH_VEL};
            else if(p->attached_dir == ATTACHED_DOWN)
                p->vel = (Vector2){0, PLAYER_LAUNCH_VEL};
            else if(p->attached_dir == ATTACHED_LEFT)
                p->vel = (Vector2){-PLAYER_LAUNCH_VEL, 0};
            else if(p->attached_dir == ATTACHED_RIGHT)
                p->vel = (Vector2){PLAYER_LAUNCH_VEL, 0};
        }
    } else {

        accel = (Vector2){0.f, 1000.f};

        if (p->power > 0.f)
        {
            if(IsKeyDown(KEY_RIGHT)) {
                accel.x += PLAYER_SPEED;
                p->power -= GetFrameTime() / 2;
            }
            if(IsKeyDown(KEY_LEFT)) {
                accel.x -= PLAYER_SPEED;
                p->power -= GetFrameTime()/ 2;
            }
            if(IsKeyDown(KEY_UP)) {
                accel.y -= PLAYER_SPEED;
                p->power -= GetFrameTime() / 2;
            }
            if(IsKeyDown(KEY_DOWN)) {
                accel.y += PLAYER_SPEED;
                p->power -= GetFrameTime() / 2;
            }

            if(IsKeyPressed(KEY_SPACE))
            {
                if(p->vel.y > 0.f)
                    p->vel.y = -PLAYER_JUMP_VEL;
                else 
                    p->vel.y -= PLAYER_JUMP_VEL;

                p->power -= 0.5f;
            }
        }

        p->vel.x += accel.x * GetFrameTime();
        p->vel.y += accel.y * GetFrameTime();

        p->pos.x += p->vel.x * GetFrameTime();
        p->pos.y += p->vel.y * GetFrameTime();

        if(p->pos.x <= PLAYER_RADIUS)
        {
            p->pos.x = PLAYER_RADIUS;
            p->vel.x = -p->vel.x * PLAYER_BOUNCE_DAMP;
        }
        else if(p->pos.x >= lvl.w-PLAYER_RADIUS)
        {
            p->pos.x = lvl.w - PLAYER_RADIUS;
            p->vel.x = -p->vel.x * PLAYER_BOUNCE_DAMP;
        }

        if(p->pos.y <= PLAYER_RADIUS)
        {
            p->pos.y = PLAYER_RADIUS;
            p->vel.y = -p->vel.y * PLAYER_BOUNCE_DAMP;
        }
        else if(p->pos.y >= lvl.h-PLAYER_RADIUS)
        {
            p->pos.y = lvl.h-PLAYER_RADIUS;
            p->vel.y = -p->vel.y * PLAYER_BOUNCE_DAMP;
        }

        // Check collision

        for(int i = 1; i < lvl.entity_system.num_elements; i++)
        {
            Entity other = lvl.entity_system.entities[i];
            if(CheckCollisionCircleRec(p->pos, PLAYER_RADIUS, other.collision))
            {
                switch(other.type)
                {
                    case SOCKET:
                    case RECT:
                        Vector2 normal = {0};

                        if(p->pos.x < other.collision.x) {
                            normal = (Vector2){-1, 0};
                        } else if(p->pos.x > other.collision.x + other.collision.width) {
                            normal = (Vector2){1, 0};
                        } else if(p->pos.y < other.collision.y) {
                            normal = (Vector2){0, -1};
                        } else {
                            normal = (Vector2){0, 1};
                        }

                        float dot = Vector2DotProduct(p->vel, normal);

                        p->vel.x -= (2 - PLAYER_BOUNCE_DAMP) * dot * normal.x;
                        p->vel.y -= (2 - PLAYER_BOUNCE_DAMP) * dot * normal.y;

                        p->pos.x += normal.x;
                        p->pos.y += normal.y;

                        break;
                    case SPIKE:
                        break;
                    default:
                        break;
                }
            }
        }
    
        if(p->power < 0.f)
            p->power = 0.f;
    }

    if(p->can_attach_timer > 0.f)
        p->can_attach_timer -= GetFrameTime();
}

void put_player_in_attached_position(EntitySystem *es, Player *p)
{
    if(p->attached_id < 0 || es->idxs[p->attached_id] >= es->num_elements)
        return;
    
    Entity *e = entity_system_get_by_id(es, p->attached_id);
    
    p->pos = e->pos;
    if(p->attached_dir == ATTACHED_UP)
        p->pos.y -= PLAYER_RADIUS + (e->size.y / 2);
    else if(p->attached_dir == ATTACHED_DOWN)
        p->pos.y += PLAYER_RADIUS + (e->size.y / 2);
    else if(p->attached_dir == ATTACHED_LEFT)
        p->pos.x -= PLAYER_RADIUS + (e->size.x / 2);
    else if(p->attached_dir == ATTACHED_RIGHT)
        p->pos.x += PLAYER_RADIUS + (e->size.x / 2);
}
