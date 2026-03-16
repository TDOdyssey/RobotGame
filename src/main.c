#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "entity.h"
#include "level.h"
#include "editor.h"

#include <stdio.h>
#include <string.h>

#define SCREEN_W 800
#define SCREEN_H 600

typedef enum ProgramMode {
    GAME,
    EDITOR,
} ProgramMode;

ProgramMode program_mode = GAME;

int main()
{
    const int screen_w = 1920;
    const int screen_h = 1080;

    InitWindow(screen_w, screen_h, "hello!!!!");
    SetTargetFPS(100);

    Level lvl;
    bool success = load_level(&lvl, "./levels/level0.txt");
    if(!success)
    {
        printf("Error: Failed to load level\n");
        return 1;
    }

    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    GuiSetStyle(DEFAULT, TEXT_SPACING, 2);
    GuiSetStyle(DEFAULT, BORDER_WIDTH, 2);

    EditorState editor_state = editor_state_init(&lvl);

    Camera2D game_camera = {0};

    game_camera.offset = (Vector2){screen_w/2, screen_h/2};
    game_camera.zoom = 1.0f;

    while (!WindowShouldClose())
    {
        game_camera.target = (Vector2){lvl.w / 2, lvl.h / 2};

        if(IsKeyPressed(KEY_F1))
        {
            if(program_mode == GAME)
                program_mode = EDITOR;
            else
                program_mode = GAME;
        }

        if(program_mode == GAME)
        {
            if(IsKeyPressed(KEY_R))
                reset_level(&lvl);

            Player *p = &lvl.p;
            update_player(lvl, p);

            BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode2D(game_camera);

            for(int i = 0; i < lvl.entity_system.num_elements; i++)
            {
                update_entity(&lvl.entity_system.entities[i]);
                draw_entity(lvl.entity_system.entities[i], false);
            }

            draw_player(lvl.p);

            DrawRectangleLines(0, 0, lvl.w, lvl.h, RED);

            EndMode2D();
            EndDrawing();
        }
        else if(program_mode == EDITOR)
            editor_state_update_and_draw(&editor_state);
    }

    CloseWindow();
    return 0;
}
